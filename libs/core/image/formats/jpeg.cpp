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
#include <yttrium/storage/source.h>
#include "../formats.h"

#include <csetjmp>
#include <cstdio> // <jpeglib.h> requires FILE declaration.

#include <jpeglib.h> // TODO: Load JPEG without libjpeg.

#ifndef NDEBUG
#	include <iomanip>
#	include <iostream>
#endif

namespace
{
	struct JpegErrorHandler
	{
		jpeg_error_mgr _error_mgr;
		std::jmp_buf _jmp_buf;
	};

	[[noreturn]] void error_callback(jpeg_common_struct* cinfo) {
		std::longjmp(reinterpret_cast<JpegErrorHandler*>(cinfo->err)->_jmp_buf, 1);
	}

#ifndef NDEBUG
	enum : std::uint8_t {
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

	class JpegDecoder
	{
	public:
		bool decode(const std::uint8_t* data, std::size_t size)
		{
			if (!decode_jpeg(data, size))
			{
				std::cerr << "<ERROR>\n";
				return false;
			}
			// TODO: Convert the loaded image to BGRA.
			return true;
		}

	private:
		static constexpr auto u16(const std::uint8_t* data) noexcept
		{
			return static_cast<std::uint16_t>(unsigned{ data[0] } << 8 | unsigned{ data[1] });
		}

		std::size_t skip_segment(const std::uint8_t* data, const std::size_t size)
		{
			if (size < 2)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size > size)
				return 0;
			return segment_size;
		}

		std::size_t decode_dht(const std::uint8_t* data, const std::size_t size)
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
			const std::uint8_t* bits = data + 3;
			std::size_t length = 0;
			for (std::size_t i = 0; i < 16; ++i)
				length += bits[i];
			if (segment_size != 19 + length)
				return 0;
			std::cerr << "DHT:\n";
			std::cerr << "\ttype=" << (type ? "ac" : "dc") << '\n';
			std::cerr << "\tid=" << id << '\n';
			std::cerr << "\tbits";
			for (std::size_t i = 0; i < 16; ++i)
				std::cerr << (i ? ',' : '=') << int{ bits[i] };
			std::cerr << '\n';
			auto huffman_values = bits + 16;
			for (std::size_t i = 0; i < 16; ++i)
			{
				if (!bits[i])
					continue;
				std::cerr << '\t';
				for (std::size_t j = 0; j < bits[i]; ++j)
					std::cerr << (j ? ',' : '\t') << int{ huffman_values[j] };
				huffman_values += bits[i];
				std::cerr << '\n';
			}
			return segment_size;
		}

		std::size_t decode_dqt(const std::uint8_t* data, const std::size_t size)
		{
			if (size < 67)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size != 67)
				return 0;
			const auto id = data[2];
			if (id > 1)
				return 0;
			for (std::size_t i = 0; i < 64; ++i)
				_quantization_tables[id][_dezigzag_table[i]] = data[3 + i];
			return segment_size;
		}

		std::size_t decode_dri(const std::uint8_t* data, const std::size_t size)
		{
			if (size < 4)
				return 0;
			const auto segment_size = u16(data);
			if (segment_size != 4)
				return 0;
			_restart_interval = u16(data + 2);
			return segment_size;
		}

		std::size_t decode_sof0(const std::uint8_t* data, const std::size_t size)
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
				const auto h = data[8 + 3 * i + 1] >> 4;
				const auto v = data[8 + 3 * i + 1] & 0xf;
				const auto qt = data[8 + 3 * i + 2];
				if (id != i + 1 || h != (i > 0 ? 1 : 2) || v != h || qt > 1)
					return 0;
				_components[i]._quantization_table = _quantization_tables[qt];
			}
			return segment_size;
		}

		std::size_t decode_sos(const std::uint8_t* data, const std::size_t size)
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
				// TODO: Setup Huffman tables.
			}
			if (data[3 + 2 * components] != 0 || data[3 + 2 * components + 1] != 63 || data[3 + 2 * components + 2] != 0)
				return 0;
			const auto payload_size = decode_payload(data + segment_size, size - segment_size);
			if (!payload_size)
				return 0;
			return segment_size;
		}

		std::size_t decode_payload(const std::uint8_t*, std::size_t)
		{
			std::cerr << "<NOT IMPLEMENTED>\n";
			return 0;
		}

		bool decode_jpeg(const std::uint8_t* data, std::size_t size)
		{
			if (size < 2 || data[0] != 0xff || data[1] != SOI)
				return false;
			data += 2;
			size -= 2;
			while (size > 0)
			{
				if (size < 2 || data[0] != 0xff)
					return false;
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
				case COM: segment_size = skip_segment(data, size); break;
				default:
					if ((marker & APP_mask) == APP)
						segment_size = skip_segment(data, size);
				}
				if (!segment_size)
					return false;
				data += segment_size;
				size -= segment_size;
			}
			return false;
		}

	private:
		struct Component
		{
			const std::uint8_t* _quantization_table = nullptr;
		};

		std::size_t _width = 0;
		std::size_t _height = 0;
		Component _components[3];
		std::uint8_t _quantization_tables[2][64];
		std::size_t _restart_interval = 0;

		static const std::uint8_t _dezigzag_table[64];
	};

	const std::uint8_t JpegDecoder::_dezigzag_table[64]{
		0, 1, 8, 16, 9, 2, 3, 10,
		17, 24, 32, 25, 18, 11, 4, 5,
		12, 19, 26, 33, 40, 48, 41, 34,
		27, 20, 13, 6, 7, 14, 21, 28,
		35, 42, 49, 56, 57, 50, 43, 36,
		29, 22, 15, 23, 30, 37, 44, 51,
		58, 59, 52, 45, 38, 31, 39, 46,
		53, 60, 61, 54, 47, 55, 62, 63
	};
#endif
}

namespace Yttrium
{
	bool read_jpeg(const Source& source, ImageInfo& info, Buffer& buffer)
	{
#ifndef NDEBUG
		{
			const auto jpeg = source.to_buffer();
			JpegDecoder{}.decode(jpeg.begin(), jpeg.size());
		}
#endif

		auto source_buffer = source.to_buffer(); // Some JPEG libraries require non-const source buffer.
		if (source_buffer.size() > std::numeric_limits<unsigned long>::max())
			return false;

		JpegErrorHandler error_handler;
		error_handler._error_mgr.error_exit = ::error_callback;
		if (setjmp(error_handler._jmp_buf))
			return false;

		jpeg_decompress_struct decompressor;
		decompressor.err = ::jpeg_std_error(&error_handler._error_mgr);
		::jpeg_create_decompress(&decompressor);
		if (setjmp(error_handler._jmp_buf))
		{
			::jpeg_destroy_decompress(&decompressor);
			return false;
		}

		::jpeg_mem_src(&decompressor, &source_buffer[0], static_cast<unsigned long>(source_buffer.size()));

		::jpeg_read_header(&decompressor, TRUE);

		decompressor.out_color_space = JCS_RGB;

		::jpeg_calc_output_dimensions(&decompressor);

		info = { decompressor.output_width, decompressor.output_height, PixelFormat::Rgb24 };

		try
		{
			buffer.reset(info.frame_size());
		}
		catch (const std::bad_alloc&)
		{
			::jpeg_destroy_decompress(&decompressor);
			throw;
		}

		::jpeg_start_decompress(&decompressor);
		for (auto scanline = &buffer[0]; decompressor.output_scanline < decompressor.output_height; scanline += info.stride())
			::jpeg_read_scanlines(&decompressor, &scanline, 1);
		::jpeg_finish_decompress(&decompressor);

		if (setjmp(error_handler._jmp_buf))
			return false;

		::jpeg_destroy_decompress(&decompressor);

		return true;
	}
}
