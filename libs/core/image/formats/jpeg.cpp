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
#	include <yttrium/storage/reader.h>
#	include <yttrium/utils/numeric.h>
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
}

namespace Yttrium
{
#ifndef NDEBUG
	namespace
	{
		constexpr std::uint16_t twocc(std::uint8_t a, std::uint8_t b) noexcept
		{
			return static_cast<std::uint16_t>(a | b << 8);
		}

		enum class JpegMarker : std::uint16_t
		{
			Sof0 = twocc(0xff, 0xc0),     // Start-of-Frame (baseline DCT).
			Sof2 = twocc(0xff, 0xc2),     // Start-of-Frame (progressive DCT).
			Dht = twocc(0xff, 0xc4),      // Define-Huffman-Tables.
			Rst = twocc(0xff, 0xd0),      // Restart (0-7).
			RstMask = twocc(0xff, 0xf8),  //
			RstValue = twocc(0x00, 0x07), //
			Soi = twocc(0xff, 0xd8),      // Start-Of-Image.
			Eoi = twocc(0xff, 0xd9),      // End-Of-Image.
			Sos = twocc(0xff, 0xda),      // Start-of-Scan.
			Dqt = twocc(0xff, 0xdb),      // Defile-Quantization-Tables.
			Dri = twocc(0xff, 0xdd),      // Define-Restart-Interval.
			App = twocc(0xff, 0xe0),      // Application (0-15).
			AppMask = twocc(0xff, 0xf0),  //
			AppValue = twocc(0x00, 0x0f), //
			Com = twocc(0xff, 0xfe),      // Comment.
		};

		constexpr JpegMarker operator&(JpegMarker a, JpegMarker b) noexcept
		{
			return static_cast<JpegMarker>(static_cast<std::uint16_t>(a) & static_cast<std::uint16_t>(b));
		}

		bool scan_jpeg(const Source& source)
		{
			Reader r{ source };

			const auto skip_segment = [&r]
			{
				std::uint16_t length;
				if (!r.read(length))
					return false;
				std::cerr << '<' << swap_bytes(length) << " bytes>\n";
				return r.skip(swap_bytes(length) - 2);
			};

			JpegMarker marker;
			if (!r.read(marker) || marker != JpegMarker::Soi)
				return false;

			std::cerr << "SOI\n";
			for (;;)
			{
				if (!r.read(marker))
					return false;
				switch (marker)
				{
				case JpegMarker::Sof0:
					std::cerr << "SOF0\n";
					if (!skip_segment())
						return false;
					break;
				case JpegMarker::Sof2:
					std::cerr << "SOF2\n";
					if (!skip_segment())
						return false;
					break;
				case JpegMarker::Dht:
					std::cerr << "DHT\n";
					if (!skip_segment())
						return false;
					break;
				case JpegMarker::Eoi:
					std::cerr << "EOI\n";
					return true;
				case JpegMarker::Sos:
					std::cerr << "SOS\n";
					if (!skip_segment())
						return false;
					break;
				case JpegMarker::Dqt:
					std::cerr << "DQT\n";
					if (!skip_segment())
						return false;
					break;
				case JpegMarker::Dri:
					std::cerr << "DRI\n";
					if (!skip_segment())
						return false;
					break;
				case JpegMarker::Com:
					std::cerr << "COM\n";
					if (!skip_segment())
						return false;
					break;
				default:
					if ((marker & JpegMarker::AppMask) == JpegMarker::App)
					{
						std::cerr << "APP" << static_cast<std::uint16_t>(marker & JpegMarker::AppValue) << '\n';
						if (!skip_segment())
							return false;
					}
					else if ((marker & JpegMarker::RstMask) == JpegMarker::Rst)
						std::cerr << "RST" << static_cast<std::uint16_t>(marker & JpegMarker::RstValue) << '\n';
					else
					{
						std::cerr << "Unknown marker 0x" << std::hex << std::setw(4) << std::setfill('0') << swap_bytes(static_cast<std::uint16_t>(marker))
							<< " @ " << std::hex << r.offset() - 2 << '\n';
						return false;
					}
				}
			}
		}
	}
#endif

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
