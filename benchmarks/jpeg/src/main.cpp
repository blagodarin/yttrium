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
#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include "../../../libs/core/src/image/formats.h"

#include <csetjmp>
#include <cstring>

#include <cstdio> // <jpeglib.h> requires FILE declaration.
#include <jpeglib.h>

#include <catch2/catch.hpp>

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

	bool read_jpeg_with_libjpeg(const void* data, std::size_t size, Yt::ImageInfo& info, Yt::Buffer& buffer, boolean do_fancy_upsampling)
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

		decompressor.out_color_space = JCS_EXT_BGRA;

		::jpeg_calc_output_dimensions(&decompressor);

		info = { decompressor.output_width, decompressor.output_height, Yt::PixelFormat::Bgra32 };

		try
		{
			buffer.reset(info.frame_size());
		}
		catch (const std::bad_alloc&)
		{
			::jpeg_destroy_decompress(&decompressor);
			throw;
		}

		decompressor.do_fancy_upsampling = do_fancy_upsampling;
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

TEST_CASE("benchmark.jpeg")
{
	const auto source = Yt::Source::from("examples/tetrium/data/textures/background.jpeg");
	REQUIRE(source);

	// Pad JPEG image for the Yttrium loader.
	auto input = source->to_buffer(2);
	input.end()[0] = 0xff;
	input.end()[1] = 0xd9;

	Yt::ImageInfo output_info;
	Yt::Buffer output;
	output.reserve(1024 * 1024 * 4);
	std::memset(output.data(), 0xff, output.capacity());

	BENCHMARK_ADVANCED("yttrium (nearest)")
	(Catch::Benchmark::Chronometer meter)
	{
		meter.measure([&input, &output_info, &output] {
			return Yt::read_jpeg(input.data(), input.size() + 2, output_info, output, Yt::Upsampling::Nearest);
		});
	};

	Yt::Image{ output_info, output.data() }.save(Yt::Writer{ "jpeg_yttrium_nearest.png" }, Yt::ImageFormat::Png);

	BENCHMARK_ADVANCED("yttrium (linear)")
	(Catch::Benchmark::Chronometer meter)
	{
		meter.measure([&input, &output_info, &output] {
			return Yt::read_jpeg(input.data(), input.size() + 2, output_info, output, Yt::Upsampling::Linear);
		});
	};

	Yt::Image{ output_info, output.data() }.save(Yt::Writer{ "jpeg_yttrium_linear.png" }, Yt::ImageFormat::Png);

	BENCHMARK_ADVANCED("libjpeg (nearest)")
	(Catch::Benchmark::Chronometer meter)
	{
		meter.measure([&input, &output_info, &output] {
			return ::read_jpeg_with_libjpeg(input.data(), input.size(), output_info, output, FALSE);
		});
	};

	Yt::Image{ output_info, output.data() }.save(Yt::Writer{ "jpeg_libjpeg_nearest.png" }, Yt::ImageFormat::Png);

	BENCHMARK_ADVANCED("libjpeg (linear)")
	(Catch::Benchmark::Chronometer meter)
	{
		meter.measure([&input, &output_info, &output] {
			return ::read_jpeg_with_libjpeg(input.data(), input.size(), output_info, output, TRUE);
		});
	};

	Yt::Image{ output_info, output.data() }.save(Yt::Writer{ "jpeg_libjpeg_linear.png" }, Yt::ImageFormat::Png);
}
