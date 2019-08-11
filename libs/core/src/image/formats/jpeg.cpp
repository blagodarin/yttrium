//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <yttrium/image/image.h>
#include <yttrium/utils/numeric.h>
#include "../../intrinsics.h"
#include "../formats.h"
#include "../utils.h"

#include <cassert>
#include <cstring>

namespace
{
	// All IDCT code is based on or taken from the 'stb_image' public domain library (https://github.com/nothings/stb).
	void idct(uint8_t* dst, size_t dst_stride, const int16_t* src) noexcept
	{
#if Y_ARCH_X86
		constexpr auto fixed = [](float x) constexpr noexcept
		{
			return Yt::fixed_point<int16_t, 12>(x);
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
		dct_pass(bias_0, 10)

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
		dct_pass(bias_1, 17)

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
#	define IDCT_1D(s0, s1, s2, s3, s4, s5, s6, s7) \
		int p2 = s2; \
		int p3 = s6; \
		int p1 = (p2 + p3) * Yt::fixed_point<int, 12>(0.5411961f); \
		int t2 = p1 + p3 * Yt::fixed_point<int, 12>(-1.847759065f); \
		int t3 = p1 + p2 * Yt::fixed_point<int, 12>(0.765366865f); \
		p2 = s0; \
		p3 = s4; \
		int t0 = (p2 + p3) * Yt::fixed_point<int, 12>(1.f); \
		int t1 = (p2 - p3) * Yt::fixed_point<int, 12>(1.f); \
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
		const int p5 = (p3 + p4) * Yt::fixed_point<int, 12>(1.175875602f); \
		t0 = t0 * Yt::fixed_point<int, 12>(0.298631336f); \
		t1 = t1 * Yt::fixed_point<int, 12>(2.053119869f); \
		t2 = t2 * Yt::fixed_point<int, 12>(3.072711026f); \
		t3 = t3 * Yt::fixed_point<int, 12>(1.501321110f); \
		p1 = p5 + p1 * Yt::fixed_point<int, 12>(-0.899976223f); \
		p2 = p5 + p2 * Yt::fixed_point<int, 12>(-2.562915447f); \
		p3 = p3 * Yt::fixed_point<int, 12>(-1.961570560f); \
		p4 = p4 * Yt::fixed_point<int, 12>(-0.390180644f); \
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

			dst[0] = Yt::clamp_to_uint8((x0 + t3) >> 17);
			dst[1] = Yt::clamp_to_uint8((x1 + t2) >> 17);
			dst[2] = Yt::clamp_to_uint8((x2 + t1) >> 17);
			dst[3] = Yt::clamp_to_uint8((x3 + t0) >> 17);
			dst[4] = Yt::clamp_to_uint8((x3 - t0) >> 17);
			dst[5] = Yt::clamp_to_uint8((x2 - t1) >> 17);
			dst[6] = Yt::clamp_to_uint8((x1 - t2) >> 17);
			dst[7] = Yt::clamp_to_uint8((x0 - t3) >> 17);
		}

#	undef IDCT_1D
#endif
	}

	enum : uint8_t
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

	struct JpegHuffmanTable
	{
		static constexpr int FastLookupBits = 10;

		const uint8_t* _values = nullptr;
		uint8_t _sizes[257]; // Bit sizes for a Huffman value at the corresponding positions.
		uint32_t _max_codes[18];
		int _delta[17];
		uint8_t _fast_lookup[1 << FastLookupBits];
	};

	struct JpegComponent
	{
		size_t _horizontal = 0;
		size_t _vertical = 0;
		const uint8_t* _quantization_table = nullptr;
		const JpegHuffmanTable* _dc_table = nullptr;
		const JpegHuffmanTable* _ac_table = nullptr;
		const int16_t* _ac_lookup = nullptr;
	};

	struct JpegData
	{
	public:
		size_t _width = 0;
		size_t _height = 0;
		const uint8_t* _quantization_tables[2];
		JpegHuffmanTable _huffman_tables[2][2];
		int16_t _ac_lookup[2][1 << JpegHuffmanTable::FastLookupBits];
		JpegComponent _components[3];
		size_t _restart_interval = 0;
		size_t _mcu_x_count = 0;
		size_t _mcu_y_count = 0;
		size_t _ycbcr_size = 0;
		size_t _ycbcr_offset[3];
		size_t _ycbcr_stride[3];

		size_t parse_headers(const uint8_t* data, size_t size) noexcept
		{
			const auto skip_segment = [&data, &size]() noexcept->size_t
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
				size_t segment_size = 0;
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
				{
					process_headers();
					return static_cast<size_t>(data - base);
				}
			}
			return 0;
		}

	private:
		static constexpr uint16_t u16(const uint8_t* data) noexcept
		{
			return static_cast<uint16_t>(unsigned{ data[0] } << 8 | unsigned{ data[1] });
		}

		size_t decode_dht(const uint8_t* data, size_t size) noexcept
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

			auto& huffman = _huffman_tables[type][id];
			huffman._values = sizes + 16;

			int value_count = 0;
			for (size_t i = 0; i < 16; ++i)
				for (uint8_t j = 0; j < sizes[i]; ++j)
					huffman._sizes[value_count++] = static_cast<uint8_t>(i + 1);

			if (segment_size != 19 + value_count)
				return 0;

			{
				uint16_t codes[256];

				int index = 0;
				for (int code = 0, i = 1; i <= 16; ++i)
				{
					huffman._delta[i] = index - code;
					if (huffman._sizes[index] == i)
					{
						while (huffman._sizes[index] == i)
							codes[index++] = static_cast<uint16_t>(code++);
						if (code - 1 >= 1 << i)
							return 0;
					}
					huffman._max_codes[i] = static_cast<uint32_t>(code) << (32 - i); // Pre-shift max code to avoid shifting bit value in JpegHuffmanTable::read.
					code <<= 1;
				}
				huffman._max_codes[17] = std::numeric_limits<uint32_t>::max();

				std::memset(huffman._fast_lookup, 0xff, sizeof huffman._fast_lookup);
				for (int i = 0; i < index; ++i)
				{
					if (const auto code_bits = int{ huffman._sizes[i] }; code_bits <= JpegHuffmanTable::FastLookupBits)
					{
						const auto offset = codes[i] << (JpegHuffmanTable::FastLookupBits - code_bits);
						const auto count = 1 << (JpegHuffmanTable::FastLookupBits - code_bits);
						for (int j = 0; j < count; ++j)
							huffman._fast_lookup[offset + j] = static_cast<uint8_t>(i);
					}
				}
			}

			if (type)
			{
				const auto ac_lookup = &_ac_lookup[id][0];
				for (int i = 0; i < 1 << JpegHuffmanTable::FastLookupBits; ++i)
				{
					ac_lookup[i] = 0;
					if (const auto index = huffman._fast_lookup[i]; index < 255)
					{
						const auto code = huffman._values[index];
						const auto code_size = huffman._sizes[index];
						const auto value_size = code & 0xf;
						if (value_size && code_size + value_size <= JpegHuffmanTable::FastLookupBits)
						{
							auto value = ((i << code_size) & ((1 << JpegHuffmanTable::FastLookupBits) - 1)) >> (JpegHuffmanTable::FastLookupBits - value_size);
							if (value < (1 << (value_size - 1)))
								value += static_cast<int>((~0u << value_size) + 1);
							if (value >= -128 && value <= 127)
								ac_lookup[i] = static_cast<int16_t>(value * 256 + (code & 0xf0) + code_size + value_size);
						}
					}
				}
			}

			return segment_size;
		}

		size_t decode_dqt(const uint8_t* data, size_t size) noexcept
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

		size_t decode_dri(const uint8_t* data, size_t size) noexcept
		{
			if (size < 4)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size != 4)
				return 0;
			_restart_interval = u16(data + 2);
			return segment_size;
		}

		size_t decode_sof0(const uint8_t* data, size_t size) noexcept
		{
			if (size < 2)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size > size || segment_size < 8)
				return 0;
			const auto color_bits = data[2];
			_width = u16(data + 3);
			_height = u16(data + 5);
			const auto components = size_t{ data[7] };
			if (color_bits != 8 || !_width || !_height || components != 3 || segment_size != 8 + 3 * components)
				return 0;
			for (size_t i = 0; i < components; ++i)
			{
				const auto id = data[8 + 3 * i];
				const auto h = size_t{ data[8 + 3 * i + 1] } >> 4;
				const auto v = size_t{ data[8 + 3 * i + 1] } & 0xf;
				const auto qt = data[8 + 3 * i + 2];
				if (id != i + 1 || h != (i > 0 ? 1u : 2u) || v != h || qt > 1)
					return 0;
				_components[i]._horizontal = h;
				_components[i]._vertical = v;
				_components[i]._quantization_table = _quantization_tables[qt];
			}
			return segment_size;
		}

		size_t decode_sos(const uint8_t* data, size_t size)
		{
			if (size < 3)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size > size)
				return 0;
			const auto components = size_t{ data[2] };
			if (components != 3 || segment_size != 6 + 2 * components)
				return 0;
			for (size_t i = 0; i < components; ++i)
			{
				const auto id = data[3 + 2 * i];
				const auto dc = data[3 + 2 * i + 1] >> 4;
				const auto ac = data[3 + 2 * i + 1] & 0xf;
				if (id != i + 1 || dc > 1 || ac > 1)
					return 0;
				_components[i]._dc_table = &_huffman_tables[0][dc];
				_components[i]._ac_table = &_huffman_tables[1][ac];
				_components[i]._ac_lookup = &_ac_lookup[ac][0];
			}
			if (data[3 + 2 * components] != 0 || data[3 + 2 * components + 1] != 63 || data[3 + 2 * components + 2] != 0)
				return 0;
			return segment_size;
		}

		void process_headers() noexcept
		{
			auto max_h = _components[0]._horizontal;
			auto max_v = _components[0]._vertical;
			for (size_t i = 1; i < 3; ++i)
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
			for (size_t i = 0; i < 3; ++i)
			{
				_ycbcr_offset[i] = _ycbcr_size;
				_ycbcr_stride[i] = _mcu_x_count * _components[i]._horizontal * 8;
				_ycbcr_size += _ycbcr_stride[i] * _mcu_y_count * _components[i]._vertical * 8;
			}
		}
	};

	class JpegBitstream
	{
	public:
		constexpr explicit JpegBitstream(const uint8_t* data) noexcept
			: _data{ data } {}

		bool read_scan(const JpegData& data, Yt::Buffer& ycbcr_buffer) noexcept
		{
			int last_dc[3]{ 0, 0, 0 };
			auto restart_counter = data._restart_interval ? data._restart_interval : std::numeric_limits<size_t>::max();
			int restart_index = 0;
			for (size_t mcu_y = 0; mcu_y < data._mcu_y_count; ++mcu_y)
			{
				for (size_t mcu_x = 0; mcu_x < data._mcu_x_count; ++mcu_x)
				{
					for (size_t c = 0; c < 3; ++c)
					{
						const auto& component = data._components[c];
						for (size_t v = 0; v < component._vertical; ++v)
						{
							for (size_t h = 0; h < component._horizontal; ++h)
							{
								alignas(16) int16_t block[64]{};
								if (!read_block(component, last_dc[c], &block[0]))
									return false;
								const auto x = (mcu_x * component._horizontal + h) * 8;
								const auto y = (mcu_y * component._vertical + v) * 8;
								uint8_t* output = ycbcr_buffer.begin() + data._ycbcr_offset[c] + y * data._ycbcr_stride[c] + x;
								::idct(output, data._ycbcr_stride[c], &block[0]);
							}
						}
					}
					if (!--restart_counter)
					{
						if (!restart(restart_index))
							return false;
						restart_counter = data._restart_interval;
						restart_index = (restart_index + 1) & 7;
					}
				}
			}
			return true;
		}

	private:
		bool read_block(const JpegComponent& component, int& last_dc, int16_t* block) noexcept
		{
			if (_free_bits > 16)
				read_bits(16);
			const auto [dc_code, dc_delta] = read_value(*component._dc_table);
			if (dc_code < 0)
				return false;
			block[0] = static_cast<int16_t>((last_dc += dc_delta) * component._quantization_table[0]);
			int i = 0;
			do
			{
				if (_free_bits > 16)
					read_bits(16);
				if (const auto fast_ac = component._ac_lookup[_buffer >> (32 - JpegHuffmanTable::FastLookupBits)]; fast_ac)
				{
					skip_bits(fast_ac & 0xf);
					i += ((fast_ac >> 4) & 0xf) + 1;
					block[_dezigzag_table[i]] = static_cast<int16_t>((fast_ac >> 8) * component._quantization_table[i]);
				}
				else
				{
					const auto [ac_code, ac_value] = read_value(*component._ac_table);
					if (ac_code <= 0)
						return !ac_code;
					const auto r = ac_code >> 4;
					const auto s = ac_code & 0xf;
					if (!s && r != 15)
						return false;
					i += r + 1;
					block[_dezigzag_table[i]] = static_cast<int16_t>(ac_value * component._quantization_table[i]);
				}
			} while (i < 63);
			return true;
		}

		bool restart(int index) noexcept
		{
			if (_free_bits > 8)
				read_bits(8);
			if ((_marker & RST_mask) != RST || (_marker & RST_value) != index)
				return false;
			_free_bits = 32;
			_buffer = 0;
			_marker = 0;
			return true;
		}

		std::pair<int, int> read_value(const JpegHuffmanTable& huffman) noexcept
		{
			assert(_free_bits <= 16);
			uint8_t code;
			if (const auto index = huffman._fast_lookup[_buffer >> (32 - JpegHuffmanTable::FastLookupBits)]; index < 255)
			{
				code = huffman._values[index];
				skip_bits(huffman._sizes[index]);
			}
			else
			{
				int size = JpegHuffmanTable::FastLookupBits + 1;
				for (;; ++size)
					if (_buffer < huffman._max_codes[size])
						break;
				if (size > 16)
					return { -1, 0 };
				code = huffman._values[static_cast<int32_t>(_buffer >> (32 - size)) + huffman._delta[size]];
				skip_bits(size);
			}
			const auto value_size = code & 0xf;
			if (!value_size)
				return { code, 0 };
			const auto bit_offset = 32 - value_size;
			if (_free_bits > bit_offset)
				read_bits(bit_offset);
			auto value = static_cast<int>(_buffer >> bit_offset);
			skip_bits(value_size);
			if (value < (1 << (value_size - 1)))
				value += static_cast<int>((~0u << value_size) + 1);
			return { code, value };
		}

		void read_bits(int max_free_bits) noexcept
		{
			assert(_free_bits > max_free_bits);
			do
			{
				auto next = _marker ? uint32_t{ 0 } : *_data++;
				if (next == 0xff)
				{
					do
					{
						next = *_data++;
					} while (next == 0xff);
					_marker = static_cast<uint8_t>(next);
					next ^= 0xff; // Negates lower bits. Produces 0xff for 0x00, stops on marker otherwise.
				}
				_free_bits -= 8;
				_buffer |= next << _free_bits;
			} while (_free_bits > max_free_bits);
		}

		void skip_bits(int count) noexcept
		{
			assert(32 - _free_bits >= count);
			_free_bits += count;
			_buffer <<= count;
		}

	private:
		const uint8_t* _data = nullptr;
		int _free_bits = 32;
		uint32_t _buffer = 0;
		uint8_t _marker = 0;

		static constexpr uint8_t _dezigzag_table[64 + 15]{
			0, 1, 8, 16, 9, 2, 3, 10,
			17, 24, 32, 25, 18, 11, 4, 5,
			12, 19, 26, 33, 40, 48, 41, 34,
			27, 20, 13, 6, 7, 14, 21, 28,
			35, 42, 49, 56, 57, 50, 43, 36,
			29, 22, 15, 23, 30, 37, 44, 51,
			58, 59, 52, 45, 38, 31, 39, 46,
			53, 60, 61, 54, 47, 55, 62, 63,
			63, 63, 63, 63, 63, 63, 63, 63, // Extra values to prevent index check (see 'read_block').
			63, 63, 63, 63, 63, 63, 63
		};
	};
}

namespace Yt
{
	bool read_jpeg(const void* data, size_t size, ImageInfo& info, Buffer& buffer, Upsampling upsampling)
	{
		const auto bytes = static_cast<const uint8_t*>(data);
		assert(size >= 2 && bytes[size - 2] == 0xff && bytes[size - 1] == EOI);
		JpegData jpeg;
		const auto scan_offset = jpeg.parse_headers(bytes, size);
		if (!scan_offset || size - scan_offset < 2)
			return false;
		Buffer ycbcr{ jpeg._ycbcr_size }; // Converting every MCU after decoding hurts performance due to worse memory access pattern.
		if (!JpegBitstream{ bytes + scan_offset }.read_scan(jpeg, ycbcr))
			return false;
		info = { jpeg._width, jpeg._height, PixelFormat::Bgra32 };
		buffer.resize(info.frame_size());
		switch (upsampling)
		{
		case Upsampling::Nearest:
		{
			YCbCrComponents components;
			components.y = ycbcr.begin() + jpeg._ycbcr_offset[0];
			components.y_stride = jpeg._ycbcr_stride[0];
			components.cb = ycbcr.begin() + jpeg._ycbcr_offset[1];
			components.cbcr_stride = jpeg._ycbcr_stride[1];
			components.cr = ycbcr.begin() + jpeg._ycbcr_offset[2];
			convert_jpeg420_to_bgra(jpeg._width, jpeg._height, components, buffer.data(), info.stride());
			break;
		}
		case Upsampling::Linear:
		{
			const auto stride = jpeg._ycbcr_stride[0];
			const auto plane_size = jpeg._height * stride;
			Buffer upsampled{ 2 * plane_size };
			upsample_2x2_linear(jpeg._width / 2, jpeg._height / 2, ycbcr.begin() + jpeg._ycbcr_offset[1], jpeg._ycbcr_stride[1], upsampled.begin(), stride);
			upsample_2x2_linear(jpeg._width / 2, jpeg._height / 2, ycbcr.begin() + jpeg._ycbcr_offset[2], jpeg._ycbcr_stride[2], upsampled.begin() + plane_size, stride);
			YCbCrComponents components;
			components.y = ycbcr.begin() + jpeg._ycbcr_offset[0];
			components.y_stride = stride;
			components.cb = upsampled.begin();
			components.cbcr_stride = stride;
			components.cr = upsampled.begin() + plane_size;
			convert_jpeg444_to_bgra(jpeg._width, jpeg._height, components, buffer.data(), info.stride());
			break;
		}
		}
		return true;
	}
}
