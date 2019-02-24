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
		std::size_t skip_segment(const std::uint8_t* data, const std::size_t size)
		{
			if (size < 2)
				return 0;
			const auto segment_size = static_cast<std::uint16_t>(data[0] << 8 | data[1]);
			if (segment_size > size)
				return 0;
			std::cerr << "\t<" << segment_size << " bytes>\n";
			return segment_size;
		}

		std::size_t decode_app(int type, const std::uint8_t* data, const std::size_t size)
		{
			std::cerr << "APP" << type << "\n";
			return skip_segment(data, size);
		}

		std::size_t decode_com(const std::uint8_t* data, const std::size_t size)
		{
			std::cerr << "COM\n";
			return skip_segment(data, size);
		}

		std::size_t decode_dht(const std::uint8_t* data, const std::size_t size)
		{
			std::cerr << "DHT\n";
			if (size < 2)
				return 0;
			const auto segment_size = static_cast<std::uint16_t>(data[0] << 8 | data[1]);
			if (segment_size > size || segment_size < 3)
				return 0;
			const auto type = data[2] >> 4;
			const auto id = data[2] & 0xf;
			if (type > 1 || id > 1)
				return 0;
			std::cerr << "\ttype=" << (type ? "ac" : "dc") << '\n';
			std::cerr << "\tid=" << id << '\n';
			return segment_size;
		}

		std::size_t decode_dqt(const std::uint8_t* data, const std::size_t size)
		{
			std::cerr << "DQT\n";
			if (size < 67)
				return 0;
			const auto segment_size = static_cast<std::uint16_t>(data[0] << 8 | data[1]);
			if (segment_size != 67)
				return 0;
			const auto id = data[2];
			if (id > 1)
				return 0;
			for (std::size_t i = 0; i < 64; ++i)
				_quantization_tables[id][_dezigzag_table[i]] = data[3 + i];
			std::cerr << "\tid=" << int{ id } << '\n';
			std::cerr << "\tqt=\n";
			for (int i = 0; i < 8; ++i)
			{
				std::cerr << '\t';
				for  (int j = 0; j < 8; ++j)
					std::cerr << '\t' << int{ _quantization_tables[id][i * 8 + j] };
				std::cerr << '\n';
			}
			return segment_size;
		}

		std::size_t decode_dri(const std::uint8_t* data, const std::size_t size)
		{
			std::cerr << "DRI\n";
			return skip_segment(data, size);
		}

		std::size_t decode_sof0(const std::uint8_t* data, const std::size_t size)
		{
			std::cerr << "SOF0\n";
			if (size < 2)
				return 0;
			const auto segment_size = static_cast<std::uint16_t>(data[0] << 8 | data[1]);
			if (segment_size > size || segment_size < 8)
				return 0;
			const auto color_bits = data[2];
			const auto width = data[3] << 8 | data[4];
			const auto height = data[5] << 8 | data[6];
			const auto components = std::size_t{ data[7] };
			if (segment_size != 8 + 3 * components)
				return 0;
			std::cerr << "\tcolor_bits=" << int{ color_bits } << '\n';
			std::cerr << "\twidth=" << width << '\n';
			std::cerr << "\theight=" << height << '\n';
			std::cerr << "\tcomponents=" << components << '\n';
			for (std::size_t i = 0; i < components; ++i)
			{
				const auto id = data[8 + 3 * i];
				const auto h = data[8 + 3 * i + 1] >> 4;
				const auto v = data[8 + 3 * i + 1] & 0xf;
				const auto qt = data[8 + 3 * i + 2];
				std::cerr << "\t[" << int{ id } << "]\n";
				std::cerr << "\t\th=" << h << '\n';
				std::cerr << "\t\tv=" << v << '\n';
				std::cerr << "\t\tqt=" << int{ qt } << '\n';
			}
			return segment_size;
		}

		std::size_t decode_sos(const std::uint8_t* data, const std::size_t size)
		{
			std::cerr << "SOS\n";
			if (size < 3)
				return 0;
			const auto segment_size = static_cast<std::uint16_t>(data[0] << 8 | data[1]);
			if (segment_size > size)
				return 0;
			const auto components = std::size_t{ data[2] };
			if (segment_size != 6 + 2 * components)
				return 0;
			for (std::size_t i = 0; i < components; ++i)
			{
				const auto id = data[3 + 2 * i];
				const auto dc = data[3 + 2 * i + 1] >> 4;
				const auto ac = data[3 + 2 * i + 1] & 0xf;
				std::cerr << "\t[" << int{ id } << "]\n";
				std::cerr << "\t\tdc=" << dc << '\n';
				std::cerr << "\t\tac=" << ac << '\n';
			}
			if (data[3 + 2 * components] != 0 || data[3 + 2 * components + 1] != 63 || data[3 + 2 * components + 2] != 0)
				return 0;
			const auto payload_size = decode_payload(data + segment_size, size - segment_size);
			if (!payload_size)
				return 0;
			return segment_size + payload_size;
		}

		std::size_t decode_payload(const std::uint8_t*, std::size_t)
		{
			std::cerr << "<NOT IMPLEMENTED>\n";
			return 0;
		}

		std::size_t decode_segment(std::uint8_t marker, const std::uint8_t* data, std::size_t size)
		{
			switch (marker)
			{
			case SOF0: return decode_sof0(data, size);
			case DHT: return decode_dht(data, size);
			case SOS: return decode_sos(data, size);
			case DQT: return decode_dqt(data, size);
			case DRI: return decode_dri(data, size);
			case COM: return decode_com(data, size);
			default:
				if ((marker & APP_mask) == APP)
					return decode_app(marker & APP_value, data, size);
				return 0;
			}
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
				if (marker == EOI)
					return true; // TODO: Return false if the buffer hasn't been decoded.
				data += 2;
				size -= 2;
				const auto segment_size = decode_segment(marker, data, size);
				if (!segment_size)
					return false; // TODO: Return true if the buffer has been decoded.
				data += segment_size;
				size -= segment_size;
			}
			return false;
		}

	private:
		std::uint8_t _quantization_tables[2][64];

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
