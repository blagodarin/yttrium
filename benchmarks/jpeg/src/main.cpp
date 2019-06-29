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

#include "../../common/include/benchmark.h"

#include <yttrium/image.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include "../../../libs/core/src/image/formats.h"

#include <csetjmp>
#include <cstring>

#include <cstdio> // <jpeglib.h> requires FILE declaration.
#include <jpeglib.h>

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

	bool read_jpeg_with_libjpeg(const void* data, std::size_t size, Yttrium::ImageInfo& info, Yttrium::Buffer& buffer, boolean do_fancy_upsampling)
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

		info = { decompressor.output_width, decompressor.output_height, Yttrium::PixelFormat::Bgra32 };

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

	// Pad JPEG image for the Yttrium loader.
	auto input = source->to_buffer(2);
	input.end()[0] = 0xff;
	input.end()[1] = 0xd9;

	Yttrium::ImageInfo output_info;
	Yttrium::Buffer output;
	output.reserve(1024 * 1024 * 4);
	std::memset(output.data(), 0xff, output.capacity());

	Benchmark<5000> benchmark;

	benchmark.add("yttrium nearest", [&input, &output_info, &output] {
		return Yttrium::read_jpeg(input.data(), input.size() + 2, output_info, output, Yttrium::Upsampling::Nearest);
	});

	Yttrium::Image{ output_info, output.data() }.save(Yttrium::Writer{ "jpeg_yttrium_nearest.png" }, Yttrium::ImageFormat::Png);

	benchmark.add("yttrium linear", [&input, &output_info, &output] {
		return Yttrium::read_jpeg(input.data(), input.size() + 2, output_info, output, Yttrium::Upsampling::Linear);
	});

	Yttrium::Image{ output_info, output.data() }.save(Yttrium::Writer{ "jpeg_yttrium_linear.png" }, Yttrium::ImageFormat::Png);

	benchmark.add("libjpeg nearest", [&input, &output_info, &output] {
		return ::read_jpeg_with_libjpeg(input.data(), input.size(), output_info, output, FALSE);
	});

	Yttrium::Image{ output_info, output.data() }.save(Yttrium::Writer{ "jpeg_libjpeg_nearest.png" }, Yttrium::ImageFormat::Png);

	benchmark.add("libjpeg linear", [&input, &output_info, &output] {
		return ::read_jpeg_with_libjpeg(input.data(), input.size(), output_info, output, TRUE);
	});

	Yttrium::Image{ output_info, output.data() }.save(Yttrium::Writer{ "jpeg_libjpeg_linear.png" }, Yttrium::ImageFormat::Png);

	benchmark.print();
}
