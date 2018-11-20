#include "../formats.h"

#include <yttrium/image.h>
#include <yttrium/storage/source.h>

#include <cstdio> // <jpeglib.h> requires FILE declaration.
#include <jpeglib.h>
#include <setjmp.h>

namespace
{
	struct JpegErrorHandler
	{
		jpeg_error_mgr _error_mgr;
		jmp_buf _jmp_buf;
	};

	[[noreturn]] void error_callback(jpeg_common_struct* cinfo) {
		longjmp(reinterpret_cast<JpegErrorHandler*>(cinfo->err)->_jmp_buf, 1);
	}
}

namespace Yttrium
{
	std::optional<ImageFormat> read_jpeg(const Source& source, Buffer& buffer)
	{
		auto source_buffer = source.to_buffer(); // Some JPEG libraries require non-const source buffer.
		if (source_buffer.size() > std::numeric_limits<unsigned long>::max())
			return {};

		JpegErrorHandler error_handler;
		error_handler._error_mgr.error_exit = ::error_callback;
		if (::setjmp(error_handler._jmp_buf))
			return {};

		jpeg_decompress_struct decompressor;
		decompressor.err = ::jpeg_std_error(&error_handler._error_mgr);
		::jpeg_create_decompress(&decompressor);
		if (::setjmp(error_handler._jmp_buf))
		{
			::jpeg_destroy_decompress(&decompressor);
			return {};
		}

		::jpeg_mem_src(&decompressor, &source_buffer[0], static_cast<unsigned long>(source_buffer.size()));

		::jpeg_read_header(&decompressor, TRUE);

		decompressor.out_color_space = JCS_RGB;

		::jpeg_calc_output_dimensions(&decompressor);

		ImageFormat format(decompressor.output_width, decompressor.output_height, PixelFormat::Rgb24);

		try
		{
			buffer.reset(format.frame_size());
		}
		catch (const std::bad_alloc&)
		{
			::jpeg_destroy_decompress(&decompressor);
			throw;
		}

		::jpeg_start_decompress(&decompressor);
		for (auto scanline = &buffer[0]; decompressor.output_scanline < decompressor.output_height; scanline += format.row_size())
			::jpeg_read_scanlines(&decompressor, &scanline, 1);
		::jpeg_finish_decompress(&decompressor);

		if (::setjmp(error_handler._jmp_buf))
			return {};

		::jpeg_destroy_decompress(&decompressor);

		return format;
	}
}
