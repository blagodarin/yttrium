//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/image.h>
#include <yttrium/utils/numeric.h>
#include "../formats.h"
#include "../utils.h"

#include <cassert>
#include <cstring>

#define USE_SSE2 __has_include(<emmintrin.h>)

#if USE_SSE2
#	include <emmintrin.h>
#endif

namespace
{
	enum : std::uint8_t
	{
		TEM = 0x01,       // Temporary.
		SOF0 = 0xc0,      // Start-of-Frame (baseline DCT).
		SOF2 = 0xc2,      // Start-of-Frame (progressive DCT).
		DHT = 0xc4,       // Define-Huffman-Tables.
		DAC = 0xcc,       // Define-Arithmetic-Coding.
		RST = 0xd0,       // Restart (0-7).
		RST_mask = 0xf8,  //
		RST_value = 0x07, //
		SOI = 0xd8,       // Start-of-Image.
		EOI = 0xd9,       // End-of-Image.
		SOS = 0xda,       // Start-of-Scan.
		DQT = 0xdb,       // Defile-Quantization-Tables.
		DNL = 0xdc,       // Define-Number-of-Lines.
		DRI = 0xdd,       // Define-Restart-Interval.
		DHP = 0xde,       //
		EXP = 0xdf,       //
		APP = 0xe0,       // Application (0-15).
		APP_mask = 0xf0,  //
		APP_value = 0x0f, //
		COM = 0xfe,       // Comment.
	};

	constexpr std::uint8_t _dezigzag_table[64 + 15]{
		0, 1, 8, 16, 9, 2, 3, 10,
		17, 24, 32, 25, 18, 11, 4, 5,
		12, 19, 26, 33, 40, 48, 41, 34,
		27, 20, 13, 6, 7, 14, 21, 28,
		35, 42, 49, 56, 57, 50, 43, 36,
		29, 22, 15, 23, 30, 37, 44, 51,
		58, 59, 52, 45, 38, 31, 39, 46,
		53, 60, 61, 54, 47, 55, 62, 63,

		// Extra values to prevent index check (see Component::read_block).
		63, 63, 63, 63, 63, 63, 63, 63,
		63, 63, 63, 63, 63, 63, 63
	};

	// All IDCT code is based on or taken from the 'stb_image' public domain library (https://github.com/nothings/stb).
	void idct(std::uint8_t* dst, std::size_t dst_stride, const std::int16_t* src) noexcept
	{
#if USE_SSE2
		constexpr auto fixed = [](float x) constexpr noexcept
		{
			return Yttrium::fixed_point<std::int16_t, 12>(x);
		};

		// This is constructed to match our regular (generic) integer IDCT exactly.
		__m128i tmp;

// dot product constant: even elems=x, odd elems=y
#	define dct_const(x, y) _mm_setr_epi16((x), (y), (x), (y), (x), (y), (x), (y))

// out(0) = c0[even]*x + c0[odd]*y   (c0, x, y 16-bit, out 32-bit)
// out(1) = c1[even]*x + c1[odd]*y
#	define dct_rot(out0, out1, x, y, c0, c1) \
		__m128i c0##lo = _mm_unpacklo_epi16((x), (y)); \
		__m128i c0##hi = _mm_unpackhi_epi16((x), (y)); \
		__m128i out0##_l = _mm_madd_epi16(c0##lo, c0); \
		__m128i out0##_h = _mm_madd_epi16(c0##hi, c0); \
		__m128i out1##_l = _mm_madd_epi16(c0##lo, c1); \
		__m128i out1##_h = _mm_madd_epi16(c0##hi, c1)

// out = in << 12  (in 16-bit, out 32-bit)
#	define dct_widen(out, in) \
		__m128i out##_l = _mm_srai_epi32(_mm_unpacklo_epi16(_mm_setzero_si128(), (in)), 4); \
		__m128i out##_h = _mm_srai_epi32(_mm_unpackhi_epi16(_mm_setzero_si128(), (in)), 4)

// wide add
#	define dct_wadd(out, a, b) \
		__m128i out##_l = _mm_add_epi32(a##_l, b##_l); \
		__m128i out##_h = _mm_add_epi32(a##_h, b##_h)

// wide sub
#	define dct_wsub(out, a, b) \
		__m128i out##_l = _mm_sub_epi32(a##_l, b##_l); \
		__m128i out##_h = _mm_sub_epi32(a##_h, b##_h)

// butterfly a/b, add bias, then shift by "s" and pack
#	define dct_bfly32o(out0, out1, a, b, bias, s) \
		{ \
			__m128i abiased_l = _mm_add_epi32(a##_l, bias); \
			__m128i abiased_h = _mm_add_epi32(a##_h, bias); \
			dct_wadd(sum, abiased, b); \
			dct_wsub(dif, abiased, b); \
			out0 = _mm_packs_epi32(_mm_srai_epi32(sum_l, s), _mm_srai_epi32(sum_h, s)); \
			out1 = _mm_packs_epi32(_mm_srai_epi32(dif_l, s), _mm_srai_epi32(dif_h, s)); \
		}

// 8-bit interleave step (for transposes)
#	define dct_interleave8(a, b) \
		tmp = a; \
		a = _mm_unpacklo_epi8(a, b); \
		b = _mm_unpackhi_epi8(tmp, b)

// 16-bit interleave step (for transposes)
#	define dct_interleave16(a, b) \
		tmp = a; \
		a = _mm_unpacklo_epi16(a, b); \
		b = _mm_unpackhi_epi16(tmp, b)

#	define dct_pass(bias, shift) \
		{ \
			/* even part */ \
			dct_rot(t2e, t3e, row2, row6, rot0_0, rot0_1); \
			__m128i sum04 = _mm_add_epi16(row0, row4); \
			__m128i dif04 = _mm_sub_epi16(row0, row4); \
			dct_widen(t0e, sum04); \
			dct_widen(t1e, dif04); \
			dct_wadd(x0, t0e, t3e); \
			dct_wsub(x3, t0e, t3e); \
			dct_wadd(x1, t1e, t2e); \
			dct_wsub(x2, t1e, t2e); \
			/* odd part */ \
			dct_rot(y0o, y2o, row7, row3, rot2_0, rot2_1); \
			dct_rot(y1o, y3o, row5, row1, rot3_0, rot3_1); \
			__m128i sum17 = _mm_add_epi16(row1, row7); \
			__m128i sum35 = _mm_add_epi16(row3, row5); \
			dct_rot(y4o, y5o, sum17, sum35, rot1_0, rot1_1); \
			dct_wadd(x4, y0o, y4o); \
			dct_wadd(x5, y1o, y5o); \
			dct_wadd(x6, y2o, y5o); \
			dct_wadd(x7, y3o, y4o); \
			dct_bfly32o(row0, row7, x0, x7, bias, shift); \
			dct_bfly32o(row1, row6, x1, x6, bias, shift); \
			dct_bfly32o(row2, row5, x2, x5, bias, shift); \
			dct_bfly32o(row3, row4, x3, x4, bias, shift); \
		}

		auto rot0_0 = dct_const(fixed(0.5411961f), fixed(0.5411961f) - fixed(1.847759065f));
		auto rot0_1 = dct_const(fixed(0.5411961f) + fixed(0.765366865f), fixed(0.5411961f));
		auto rot1_0 = dct_const(fixed(1.175875602f) - fixed(0.899976223f), fixed(1.175875602f));
		auto rot1_1 = dct_const(fixed(1.175875602f), fixed(1.175875602f) - fixed(2.562915447f));
		auto rot2_0 = dct_const(fixed(0.298631336f) - fixed(1.961570560f), -fixed(1.961570560f));
		auto rot2_1 = dct_const(-fixed(1.961570560f), fixed(3.072711026f) - fixed(1.961570560f));
		auto rot3_0 = dct_const(fixed(2.053119869f) - fixed(0.390180644f), -fixed(0.390180644f));
		auto rot3_1 = dct_const(-fixed(0.390180644f), fixed(1.501321110f) - fixed(0.390180644f));

		// rounding biases in column/row passes, see stbi__idct_block for explanation.
		auto bias_0 = _mm_set1_epi32(512);
		auto bias_1 = _mm_set1_epi32(65536 + (128 << 17));

		// load
		auto row0 = _mm_load_si128(reinterpret_cast<const __m128i*>(src) + 0);
		auto row1 = _mm_load_si128(reinterpret_cast<const __m128i*>(src) + 1);
		auto row2 = _mm_load_si128(reinterpret_cast<const __m128i*>(src) + 2);
		auto row3 = _mm_load_si128(reinterpret_cast<const __m128i*>(src) + 3);
		auto row4 = _mm_load_si128(reinterpret_cast<const __m128i*>(src) + 4);
		auto row5 = _mm_load_si128(reinterpret_cast<const __m128i*>(src) + 5);
		auto row6 = _mm_load_si128(reinterpret_cast<const __m128i*>(src) + 6);
		auto row7 = _mm_load_si128(reinterpret_cast<const __m128i*>(src) + 7);

		// column pass
		dct_pass(bias_0, 10);

		{
			// 16bit 8x8 transpose pass 1
			dct_interleave16(row0, row4);
			dct_interleave16(row1, row5);
			dct_interleave16(row2, row6);
			dct_interleave16(row3, row7);

			// transpose pass 2
			dct_interleave16(row0, row2);
			dct_interleave16(row1, row3);
			dct_interleave16(row4, row6);
			dct_interleave16(row5, row7);

			// transpose pass 3
			dct_interleave16(row0, row1);
			dct_interleave16(row2, row3);
			dct_interleave16(row4, row5);
			dct_interleave16(row6, row7);
		}

		// row pass
		dct_pass(bias_1, 17);

		{
			// pack
			auto p0 = _mm_packus_epi16(row0, row1); // a0a1a2a3...a7b0b1b2b3...b7
			auto p1 = _mm_packus_epi16(row2, row3);
			auto p2 = _mm_packus_epi16(row4, row5);
			auto p3 = _mm_packus_epi16(row6, row7);

			// 8bit 8x8 transpose pass 1
			dct_interleave8(p0, p2); // a0e0a1e1...
			dct_interleave8(p1, p3); // c0g0c1g1...

			// transpose pass 2
			dct_interleave8(p0, p1); // a0c0e0g0...
			dct_interleave8(p2, p3); // b0d0f0h0...

			// transpose pass 3
			dct_interleave8(p0, p2); // a0b0c0d0...
			dct_interleave8(p1, p3); // a4b4c4d4...

			// store
			_mm_storel_epi64(reinterpret_cast<__m128i*>(dst), p0);
			dst += dst_stride;
			_mm_storel_epi64(reinterpret_cast<__m128i*>(dst), _mm_shuffle_epi32(p0, 0x4e));
			dst += dst_stride;
			_mm_storel_epi64(reinterpret_cast<__m128i*>(dst), p2);
			dst += dst_stride;
			_mm_storel_epi64(reinterpret_cast<__m128i*>(dst), _mm_shuffle_epi32(p2, 0x4e));
			dst += dst_stride;
			_mm_storel_epi64(reinterpret_cast<__m128i*>(dst), p1);
			dst += dst_stride;
			_mm_storel_epi64(reinterpret_cast<__m128i*>(dst), _mm_shuffle_epi32(p1, 0x4e));
			dst += dst_stride;
			_mm_storel_epi64(reinterpret_cast<__m128i*>(dst), p3);
			dst += dst_stride;
			_mm_storel_epi64(reinterpret_cast<__m128i*>(dst), _mm_shuffle_epi32(p3, 0x4e));
		}

#	undef dct_const
#	undef dct_rot
#	undef dct_widen
#	undef dct_wadd
#	undef dct_wsub
#	undef dct_bfly32o
#	undef dct_interleave8
#	undef dct_interleave16
#	undef dct_pass
#else
		using Yttrium::fixed_point;

#	define IDCT_1D(s0, s1, s2, s3, s4, s5, s6, s7) \
		int p2 = s2; \
		int p3 = s6; \
		int p1 = (p2 + p3) * fixed_point<int, 12>(0.5411961f); \
		int t2 = p1 + p3 * fixed_point<int, 12>(-1.847759065f); \
		int t3 = p1 + p2 * fixed_point<int, 12>(0.765366865f); \
		p2 = s0; \
		p3 = s4; \
		int t0 = (p2 + p3) * fixed_point<int, 12>(1.f); \
		int t1 = (p2 - p3) * fixed_point<int, 12>(1.f); \
		int x0 = t0 + t3; \
		int x3 = t0 - t3; \
		int x1 = t1 + t2; \
		int x2 = t1 - t2; \
		t0 = s7; \
		t1 = s5; \
		t2 = s3; \
		t3 = s1; \
		p3 = t0 + t2; \
		int p4 = t1 + t3; \
		p1 = t0 + t3; \
		p2 = t1 + t2; \
		const int p5 = (p3 + p4) * fixed_point<int, 12>(1.175875602f); \
		t0 = t0 * fixed_point<int, 12>(0.298631336f); \
		t1 = t1 * fixed_point<int, 12>(2.053119869f); \
		t2 = t2 * fixed_point<int, 12>(3.072711026f); \
		t3 = t3 * fixed_point<int, 12>(1.501321110f); \
		p1 = p5 + p1 * fixed_point<int, 12>(-0.899976223f); \
		p2 = p5 + p2 * fixed_point<int, 12>(-2.562915447f); \
		p3 = p3 * fixed_point<int, 12>(-1.961570560f); \
		p4 = p4 * fixed_point<int, 12>(-0.390180644f); \
		t3 += p1 + p4; \
		t2 += p2 + p3; \
		t1 += p2 + p4; \
		t0 += p1 + p3

		int buffer[64];

		for (int* b = &buffer[0]; b != &buffer[8]; ++src, ++b)
		{
			if (src[8] == 0 && src[16] == 0 && src[24] == 0 && src[32] == 0 && src[40] == 0 && src[48] == 0 && src[56] == 0)
			{
				b[0] = b[8] = b[16] = b[24] = b[32] = b[40] = b[48] = b[56] = src[0] * 4;
				continue;
			}

			IDCT_1D(src[0], src[8], src[16], src[24], src[32], src[40], src[48], src[56]);

			x0 += 512;
			x1 += 512;
			x2 += 512;
			x3 += 512;

			b[0] = (x0 + t3) >> 10;
			b[8] = (x1 + t2) >> 10;
			b[16] = (x2 + t1) >> 10;
			b[24] = (x3 + t0) >> 10;
			b[32] = (x3 - t0) >> 10;
			b[40] = (x2 - t1) >> 10;
			b[48] = (x1 - t2) >> 10;
			b[56] = (x0 - t3) >> 10;
		}

		for (const int* b = &buffer[0]; b != &buffer[64]; b += 8, dst += dst_stride)
		{
			IDCT_1D(b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);

			x0 += 65536 + (128 << 17);
			x1 += 65536 + (128 << 17);
			x2 += 65536 + (128 << 17);
			x3 += 65536 + (128 << 17);

			using Yttrium::clamp_to_uint8;
			dst[0] = clamp_to_uint8((x0 + t3) >> 17);
			dst[1] = clamp_to_uint8((x1 + t2) >> 17);
			dst[2] = clamp_to_uint8((x2 + t1) >> 17);
			dst[3] = clamp_to_uint8((x3 + t0) >> 17);
			dst[4] = clamp_to_uint8((x3 - t0) >> 17);
			dst[5] = clamp_to_uint8((x2 - t1) >> 17);
			dst[6] = clamp_to_uint8((x1 - t2) >> 17);
			dst[7] = clamp_to_uint8((x0 - t3) >> 17);
		}

#	undef IDCT_1D
#endif
	}

	struct JpegBitstream
	{
		const std::uint8_t* _data = nullptr;
		const std::uint8_t* const _end;
		int _free_bits = 32;
		std::uint32_t _buffer = 0;

		constexpr JpegBitstream(const std::uint8_t* data, std::size_t size) noexcept
			: _data{ data }, _end{ data + size } {}

		void read_until(int max_free_bits) noexcept
		{
			assert(_free_bits > max_free_bits);
			do
			{
				if (_data == _end)
				{
					_free_bits -= 8;
					_buffer |= 255u << _free_bits;
					continue;
				}
				const auto next = *_data++;
				_free_bits -= 8;
				_buffer |= std::uint32_t{ next } << _free_bits;
				if (next == 0xff && _data != _end)
				{
					switch (const auto next_next = *_data++; next_next)
					{
					case 0x00: break; // 0xff is followed by 0x00 to prevent forming a valid marker.
					case EOI: _data = _end; break;
					case 0xff: break; // 0xff may be used as a filler before a valid marker.
					default:
						if ((next_next & RST_mask) == RST)
						{
							_free_bits -= 8;
							_buffer |= std::uint32_t{ next_next } << _free_bits;
						}
						else
							_data = _end;
					}
				}
			} while (_free_bits > max_free_bits);
		}

		void skip_bits(int count) noexcept
		{
			assert(_free_bits <= 32 - count);
			_free_bits += count;
			_buffer <<= count;
		}
	};

	class JpegDecoder
	{
	public:
		bool decode(const std::uint8_t* data, std::size_t size, Yttrium::ImageInfo& info, Yttrium::Buffer& buffer)
		{
			if (const auto parsed = parse_headers(data, size); parsed > 0)
			{
				process_headers();
				assert(_ycbcr_stride[1] == _ycbcr_stride[2]);
				Yttrium::Buffer ycbcr_buffer{ _ycbcr_size }; // Converting every MCU after decoding hurts performance due to worse memory access pattern.
				if (parse_payload(ycbcr_buffer, data + parsed, size - parsed))
				{
					info = { _width, _height, Yttrium::PixelFormat::Bgra32 };
					buffer.resize(info.frame_size());
					Yttrium::YCbCrComponents components;
					components.y = ycbcr_buffer.begin() + _ycbcr_offset[0];
					components.y_stride = _ycbcr_stride[0];
					components.cb = ycbcr_buffer.begin() + _ycbcr_offset[1];
					components.cbcr_stride = _ycbcr_stride[1];
					components.cr = ycbcr_buffer.begin() + _ycbcr_offset[2];
					Yttrium::convert_jpeg420_to_bgra(_width, _height, components, buffer.data(), info.stride());
					return true;
				}
			}
			return false;
		}

	private:
		static constexpr auto u16(const std::uint8_t* data) noexcept
		{
			return static_cast<std::uint16_t>(unsigned{ data[0] } << 8 | unsigned{ data[1] });
		}

		std::size_t decode_dht(const std::uint8_t* data, std::size_t size) noexcept
		{
			if (size < 2)
				return 0;

			const auto segment_size = u16(data);
			if (segment_size > size || segment_size < 19)
				return 0;

			const auto type = data[2] >> 4;
			const auto id = data[2] & 0xf;
			if (type > 1 || id > 1)
				return 0;

			const auto* const sizes = data + 3;

			HuffmanTable& huffman = _huffman_tables[type][id];
			huffman._values = sizes + 16;

			int value_count = 0;
			for (std::size_t i = 0; i < 16; ++i)
				for (std::uint8_t j = 0; j < sizes[i]; ++j)
					huffman._sizes[value_count++] = static_cast<std::uint8_t>(i + 1);

			if (segment_size != 19 + value_count)
				return 0;

			int index = 0;
			for (int code = 0, i = 1; i <= 16; ++i)
			{
				huffman._delta[i] = index - code;
				if (huffman._sizes[index] == i)
				{
					while (huffman._sizes[index] == i)
						huffman._codes[index++] = static_cast<std::uint16_t>(code++);
					if (code - 1 >= 1 << i)
						return 0;
				}
				huffman._max_codes[i] = static_cast<std::uint32_t>(code) << (32 - i); // Pre-shift max code to avoid shifting bit value in HuffmanTable::read.
				code <<= 1;
			}
			huffman._max_codes[17] = std::numeric_limits<std::uint32_t>::max();

			std::memset(huffman._lookup, 0xff, sizeof huffman._lookup);
			for (int i = 0; i < index; ++i)
			{
				if (const auto code_bits = int{ huffman._sizes[i] }; code_bits <= HuffmanTable::LookupBits)
				{
					const auto offset = huffman._codes[i] << (HuffmanTable::LookupBits - code_bits);
					const auto count = 1 << (HuffmanTable::LookupBits - code_bits);
					for (int j = 0; j < count; ++j)
						huffman._lookup[offset + j] = static_cast<std::uint8_t>(i);
				}
			}

			return segment_size;
		}

		std::size_t decode_dqt(const std::uint8_t* data, std::size_t size) noexcept
		{
			if (size < 67)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size != 67)
				return 0;
			const auto id = data[2];
			if (id > 1)
				return 0;
			_quantization_tables[id] = data + 3;
			return segment_size;
		}

		std::size_t decode_dri(const std::uint8_t* data, std::size_t size) noexcept
		{
			if (size < 4)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size != 4)
				return 0;
			_restart_interval = u16(data + 2);
			return segment_size;
		}

		std::size_t decode_sof0(const std::uint8_t* data, std::size_t size) noexcept
		{
			if (size < 2)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size > size || segment_size < 8)
				return 0;
			const auto color_bits = data[2];
			_width = u16(data + 3);
			_height = u16(data + 5);
			const auto components = std::size_t{ data[7] };
			if (color_bits != 8 || !_width || !_height || components != 3 || segment_size != 8 + 3 * components)
				return 0;
			for (std::size_t i = 0; i < components; ++i)
			{
				const auto id = data[8 + 3 * i];
				const auto h = std::size_t{ data[8 + 3 * i + 1] } >> 4;
				const auto v = std::size_t{ data[8 + 3 * i + 1] } & 0xf;
				const auto qt = data[8 + 3 * i + 2];
				if (id != i + 1 || h != (i > 0 ? 1 : 2) || v != h || qt > 1)
					return 0;
				_components[i]._horizontal = h;
				_components[i]._vertical = v;
				_components[i]._quantization_table = _quantization_tables[qt];
			}
			return segment_size;
		}

		std::size_t decode_sos(const std::uint8_t* data, std::size_t size)
		{
			if (size < 3)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size > size)
				return 0;
			const auto components = std::size_t{ data[2] };
			if (components != 3 || segment_size != 6 + 2 * components)
				return 0;
			for (std::size_t i = 0; i < components; ++i)
			{
				const auto id = data[3 + 2 * i];
				const auto dc = data[3 + 2 * i + 1] >> 4;
				const auto ac = data[3 + 2 * i + 1] & 0xf;
				if (id != i + 1 || dc > 1 || ac > 1)
					return 0;
				_components[i]._dc_table = &_huffman_tables[0][dc];
				_components[i]._ac_table = &_huffman_tables[1][ac];
			}
			if (data[3 + 2 * components] != 0 || data[3 + 2 * components + 1] != 63 || data[3 + 2 * components + 2] != 0)
				return 0;
			return segment_size;
		}

		std::size_t parse_headers(const std::uint8_t* data, std::size_t size) noexcept
		{
			const auto skip_segment = [&data, &size]() noexcept->std::size_t
			{
				if (size < 2)
					return 0;
				const auto segment_size = u16(data);
				if (segment_size > size)
					return 0;
				return segment_size;
			};

			if (size < 2 || data[0] != 0xff || data[1] != SOI)
				return 0;
			const auto base = data;
			data += 2;
			size -= 2;
			while (size > 0)
			{
				if (size < 2 || data[0] != 0xff)
					return 0;
				const auto marker = data[1];
				data += 2;
				size -= 2;
				std::size_t segment_size = 0;
				switch (marker)
				{
				case SOF0: segment_size = decode_sof0(data, size); break;
				case DHT: segment_size = decode_dht(data, size); break;
				case SOS: segment_size = decode_sos(data, size); break;
				case DQT: segment_size = decode_dqt(data, size); break;
				case DRI: segment_size = decode_dri(data, size); break;
				case COM: segment_size = skip_segment(); break;
				default:
					if ((marker & APP_mask) == APP)
						segment_size = skip_segment();
				}
				if (!segment_size)
					return 0;
				data += segment_size;
				size -= segment_size;
				if (marker == SOS)
					return static_cast<std::size_t>(data - base);
			}
			return 0;
		}

		void process_headers() noexcept
		{
			auto max_h = _components[0]._horizontal;
			auto max_v = _components[0]._vertical;
			for (std::size_t i = 1; i < 3; ++i)
			{
				if (const auto h = _components[i]._horizontal; h > max_h)
					max_h = h;
				if (const auto v = _components[i]._vertical; v > max_v)
					max_v = v;
			}
			const auto mcu_width = max_h * 8;
			const auto mcu_height = max_v * 8;
			_mcu_x_count = (_width + mcu_width - 1) / mcu_width;
			_mcu_y_count = (_height + mcu_height - 1) / mcu_height;
			_ycbcr_size = 0;
			for (std::size_t i = 0; i < 3; ++i)
			{
				_ycbcr_offset[i] = _ycbcr_size;
				_ycbcr_stride[i] = _mcu_x_count * _components[i]._horizontal * 8;
				_ycbcr_size += _ycbcr_stride[i] * _mcu_y_count * _components[i]._vertical * 8;
			}
		}

		bool parse_payload(Yttrium::Buffer& ycbcr_buffer, const std::uint8_t* data, std::size_t size) noexcept
		{
			int last_dc[3]{ 0, 0, 0 };
			JpegBitstream bitstream{ data, size };
			for (std::size_t mcu_y = 0; mcu_y < _mcu_y_count; ++mcu_y)
			{
				for (std::size_t mcu_x = 0; mcu_x < _mcu_x_count; ++mcu_x)
				{
					for (std::size_t c = 0; c < 3; ++c)
					{
						const auto& component = _components[c];
						for (std::size_t v = 0; v < component._vertical; ++v)
						{
							for (std::size_t h = 0; h < component._horizontal; ++h)
							{
								std::int16_t block[64]{};
								if (!component.read_block(&block[0], bitstream, last_dc[c]))
									return false;
								const auto x = (mcu_x * component._horizontal + h) * 8;
								const auto y = (mcu_y * component._vertical + v) * 8;
								std::uint8_t* output = ycbcr_buffer.begin() + _ycbcr_offset[c] + y * _ycbcr_stride[c] + x;
								::idct(output, _ycbcr_stride[c], &block[0]);
							}
						}
					}
				}
			}
			return true;
		}

	private:
		struct HuffmanTable
		{
			static constexpr int LookupBits = 9;

			const std::uint8_t* _values = nullptr;
			std::uint8_t _sizes[257]; // Bit sizes for a Huffman value at the corresponding positions.
			std::uint16_t _codes[256];
			std::uint32_t _max_codes[18];
			int _delta[17];
			std::uint8_t _lookup[1 << LookupBits];

			std::pair<int, int> read(JpegBitstream& bitstream) const noexcept
			{
				if (bitstream._free_bits > 16)
					bitstream.read_until(16);
				std::uint8_t code;
				if (const auto index = _lookup[bitstream._buffer >> (32 - LookupBits)]; index < 255)
				{
					code = _values[index];
					bitstream.skip_bits(_sizes[index]);
				}
				else
				{
					int size = LookupBits + 1;
					for (;; ++size)
						if (bitstream._buffer < _max_codes[size])
							break;
					if (size > 16)
						return { -1, 0 };
					code = _values[static_cast<std::int32_t>(bitstream._buffer >> (32 - size)) + _delta[size]];
					bitstream.skip_bits(size);
				}
				const auto length = code & 0xf;
				if (!length)
					return { code, 0 };
				const auto bit_offset = 32 - length;
				if (bitstream._free_bits > bit_offset)
					bitstream.read_until(bit_offset);
				auto value = static_cast<int>(bitstream._buffer >> bit_offset);
				bitstream.skip_bits(length);
				if (value < (1 << (length - 1)))
					value += static_cast<int>((std::numeric_limits<unsigned>::max() << length) + 1); // (-1 << length) + 1
				return { code, value };
			}
		};

		struct Component
		{
			std::size_t _horizontal = 0;
			std::size_t _vertical = 0;
			const std::uint8_t* _quantization_table = nullptr;
			const HuffmanTable* _dc_table = nullptr;
			const HuffmanTable* _ac_table = nullptr;

			bool read_block(std::int16_t* block, JpegBitstream& bitstream, int& last_dc) const noexcept
			{
				const auto [dc_code, dc_delta] = _dc_table->read(bitstream);
				if (dc_code < 0)
					return false;
				block[0] = static_cast<std::int16_t>((last_dc += dc_delta) * _quantization_table[0]);
				int i = 0;
				do
				{
					const auto [ac_code, ac_value] = _ac_table->read(bitstream);
					if (ac_code <= 0)
						return !ac_code;
					const auto r = ac_code >> 4;
					const auto s = ac_code & 0xf;
					if (!s && r != 15)
						return false;
					i += r + 1;
					block[_dezigzag_table[i]] = static_cast<std::int16_t>(ac_value * _quantization_table[i]);
				} while (i < 63);
				return true;
			}
		};

		std::size_t _width = 0;
		std::size_t _height = 0;
		Component _components[3];
		const std::uint8_t* _quantization_tables[2];
		HuffmanTable _huffman_tables[2][2];
		std::size_t _restart_interval = 0;
		std::size_t _mcu_x_count = 0;
		std::size_t _mcu_y_count = 0;
		std::size_t _ycbcr_size = 0;
		std::size_t _ycbcr_offset[3];
		std::size_t _ycbcr_stride[3];
	};
}

namespace Yttrium
{
	bool read_jpeg(const void* data, std::size_t size, ImageInfo& info, Buffer& buffer)
	{
		return JpegDecoder{}.decode(static_cast<const std::uint8_t*>(data), size, info, buffer);
	}
}
