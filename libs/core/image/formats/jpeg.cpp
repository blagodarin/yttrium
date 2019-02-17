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

	bool scan_jpeg(const Yttrium::Source& source)
	{
		const auto buffer = source.to_buffer();

		auto data = buffer.begin();
		auto size = buffer.size();

		if (size < 2 || data[0] != 0xff || data[1] != SOI)
			return false;

		std::cerr << "SOI\n";

		data += 2;
		size -= 2;

		const auto skip_segment = [&data, &size] {
			if (size < 2)
				return false;
			const auto segment_size = static_cast<std::size_t>(data[0] << 8 | data[1]);
			if (segment_size > size)
				return false;
			std::cerr << '<' << segment_size << " bytes>\n";
			data += segment_size;
			size -= segment_size;
			return true;
		};

		const auto decode_dht = [&data, &size] {
			if (size < 2)
				return false;
			const auto segment_size = static_cast<std::size_t>(data[0] << 8 | data[1]);
			if (segment_size > size || segment_size < 3)
				return false;
			const auto type = data[2] >> 4;
			const auto id = data[2] & 0xf;
			if (type > 1 || id > 1)
				return false;
			std::cerr << "\ttype=" << (type ? "ac" : "dc") << '\n';
			std::cerr << "\tid=" << id << '\n';
			data += segment_size;
			size -= segment_size;
			return true;
		};

		const auto decode_dqt = [&data, &size] {
			if (size < 67)
				return false;
			const auto segment_size = static_cast<std::size_t>(data[0] << 8 | data[1]);
			if (segment_size != 67)
				return false;
			const auto id = int{ data[2] };
			if (id > 3)
				return false;
			std::cerr << "\tid=" << id << '\n';
			data += segment_size;
			size -= segment_size;
			return true;
		};

		const auto decode_sof0 = [&data, &size] {
			if (size < 2)
				return false;
			const auto segment_size = static_cast<std::size_t>(data[0] << 8 | data[1]);
			if (segment_size > size || segment_size < 8)
				return false;
			const auto color_bits = int{ data[2] };
			const auto width = data[3] << 8 | data[4];
			const auto height = data[5] << 8 | data[6];
			const auto components = std::size_t{ data[7] };
			std::cerr << "\tcolor_bits=" << color_bits << '\n';
			std::cerr << "\twidth=" << width << '\n';
			std::cerr << "\theight=" << height << '\n';
			std::cerr << "\tcomponents=" << components << '\n';
			if (segment_size != 8 + 3 * components)
				return false;
			for (std::size_t i = 0; i < components; ++i)
			{
				const auto id = int{ data[8 + 3 * i] };
				const auto h = int{ data[8 + 3 * i + 1] } >> 4;
				const auto v = int{ data[8 + 3 * i + 1] } & 0xf;
				const auto qt = int{ data[8 + 3 * i + 2] };
				std::cerr << "\t[" << id << "]\n";
				std::cerr << "\t\th=" << h << '\n';
				std::cerr << "\t\tv=" << v << '\n';
				std::cerr << "\t\tqt=" << qt << '\n';
			}
			data += segment_size;
			size -= segment_size;
			return true;
		};

		const auto decode_sos = [&data, &size] {
			if (size < 3)
				return false;
			const auto segment_size = static_cast<std::size_t>(data[0] << 8 | data[1]);
			if (segment_size > size)
				return false;
			const auto components = std::size_t{ data[2] };
			if (segment_size != 6 + 2 * components)
				return false;
			for (std::size_t i = 0; i < components; ++i)
			{
				const auto id = int{ data[3 + 2 * i] };
				const auto dc = int{ data[3 + 2 * i + 1] } >> 4;
				const auto ac = int{ data[3 + 2 * i + 1] } & 0xf;
				std::cerr << "\t[" << id << "]\n";
				std::cerr << "\t\tdc=" << dc << '\n';
				std::cerr << "\t\tac=" << ac << '\n';
			}
			if (data[3 + 2 * components] != 0 || data[3 + 2 * components + 1] != 63 || data[3 + 2 * components + 2] != 0)
				return false;
			data += segment_size;
			size -= segment_size;
			std::cerr << "<NOT IMPLEMENTED>\n";
			return false;
		};

		for (;;)
		{
			if (size < 2 || data[0] != 0xff)
				return false;
			const auto marker = data[1];
			data += 2;
			size -= 2;
			switch (marker)
			{
			case SOF0:
				std::cerr << "SOF0\n";
				if (!decode_sof0())
					return false;
				break;
			case SOF2:
				std::cerr << "SOF2\n";
				if (!skip_segment())
					return false;
				break;
			case DHT:
				std::cerr << "DHT\n";
				if (!decode_dht())
					return false;
				break;
			case EOI:
				std::cerr << "EOI\n";
				return true;
			case SOS:
				std::cerr << "SOS\n";
				if (!decode_sos())
					return false;
				break;
			case DQT:
				std::cerr << "DQT\n";
				if (!decode_dqt())
					return false;
				break;
			case DRI:
				std::cerr << "DRI\n";
				if (!skip_segment())
					return false;
				break;
			case COM:
				std::cerr << "COM\n";
				if (!skip_segment())
					return false;
				break;
			default:
				if ((marker & APP_mask) == APP)
				{
					std::cerr << "APP" << (marker & APP_value) << '\n';
					if (!skip_segment())
						return false;
				}
				else if ((marker & RST_mask) == RST)
					std::cerr << "RST" << (marker & RST_value) << '\n';
				else
					return false;
			}
		}
	}
#endif
}

namespace Yttrium
{
	bool read_jpeg(const Source& source, ImageInfo& info, Buffer& buffer)
	{
#ifndef NDEBUG
		if (!scan_jpeg(source))
			std::cerr << "<ERROR>\n";
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
