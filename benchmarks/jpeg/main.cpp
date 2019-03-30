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
#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include "../../libs/core/image/formats.h"

#include <chrono>
#include <cmath>
#include <csetjmp>
#include <cstdio> // <jpeglib.h> requires FILE declaration.
#include <cstring>
#include <iostream>

#include <jpeglib.h>

namespace
{
	struct JpegErrorHandler
	{
		jpeg_error_mgr _error_mgr;
		std::jmp_buf _jmp_buf;
	};

	[[noreturn]] void error_callback(jpeg_common_struct* cinfo)
	{
		std::longjmp(reinterpret_cast<JpegErrorHandler*>(cinfo->err)->_jmp_buf, 1);
	}

	bool read_jpeg_with_libjpeg(const void* data, std::size_t size, Yttrium::ImageInfo& info, Yttrium::Buffer& buffer)
	{
		if (size > std::numeric_limits<unsigned long>::max())
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

		// Some JPEG libraries require non-const source buffer.
		::jpeg_mem_src(&decompressor, static_cast<unsigned char*>(const_cast<void*>(data)), static_cast<unsigned long>(size));

		::jpeg_read_header(&decompressor, TRUE);

		decompressor.out_color_space = JCS_RGB;

		::jpeg_calc_output_dimensions(&decompressor);

		info = { decompressor.output_width, decompressor.output_height, Yttrium::PixelFormat::Rgb24 };

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

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "No file name specified\n";
		return 1;
	}

	const auto source = Yttrium::Source::from(argv[1]);
	if (!source)
	{
		std::cerr << "Unable to open \"" << argv[1] << "\"\n";
		return 1;
	}

	const auto input = source->to_buffer();

	Yttrium::ImageInfo output_info;
	Yttrium::Buffer output;
	output.reserve(1024 * 1024 * 4);
	std::memset(output.data(), 0xff, output.capacity());

	std::chrono::microseconds duration;

	auto start_time = std::chrono::high_resolution_clock::now();
	int yttrium_iterations = 0;
	for (;;)
	{
		if (!Yttrium::read_jpeg(input.data(), input.size(), output_info, output))
		{
			std::cerr << "Unsupported JPEG image\n";
			return 1;
		}
		++yttrium_iterations;
		duration = std::chrono::duration_cast<decltype(duration)>(std::chrono::high_resolution_clock::now() - start_time);
		if (duration > std::chrono::seconds{ 1 })
			break;
	}
	const auto yttrium_time = static_cast<double>(duration.count());

	Yttrium::Image{ output_info, output.data() }.save(Yttrium::Writer{ "yttrium_output.png" }, Yttrium::ImageFormat::Png);

	start_time = std::chrono::high_resolution_clock::now();
	int libjpeg_iterations = 0;
	for (;;)
	{
		if (!::read_jpeg_with_libjpeg(input.data(), input.size(), output_info, output))
		{
			std::cerr << "Invalid JPEG image\n"; // Shouldn't happen since we're unlikely to support JPEG format better than libjpeg.
			return 1;
		}
		++libjpeg_iterations;
		duration = std::chrono::duration_cast<decltype(duration)>(std::chrono::high_resolution_clock::now() - start_time);
		if (duration > std::chrono::seconds{ 1 })
			break;
	}
	const auto libjpeg_time = static_cast<double>(duration.count());

	Yttrium::Image{ output_info, output.data() }.save(Yttrium::Writer{ "libjpeg_output.png" }, Yttrium::ImageFormat::Png);

	// Yttrium outputs BGRA images and libjpeg produces RGB images, so it is OK for Yttrium to be 33% slower.
	std::cerr << "Yttrium: " << std::lround(yttrium_time / yttrium_iterations) << " us/image, " << std::lround(yttrium_iterations * 1'000'000.0 / yttrium_time) << " images/s\n";
	std::cerr << "libjpeg: " << std::lround(libjpeg_time / libjpeg_iterations) << " us/image, " << std::lround(libjpeg_iterations * 1'000'000.0 / libjpeg_time) << " images/s (" << std::fixed << (libjpeg_iterations * yttrium_time) / (yttrium_iterations * libjpeg_time) << " times faster)\n";
	return 0;
}
