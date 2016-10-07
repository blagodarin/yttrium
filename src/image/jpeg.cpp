#include "jpeg.h"

#include <yttrium/image.h>
#include <yttrium/io/file.h>

#include <new>

#include <cstdio> // <jpeglib.h> requires FILE declaration.
#include <jpeglib.h>
#include <setjmp.h>

// TODO: Substitute JpegLib's memory manager.

namespace
{
	struct JpegErrorHandler
	{
		jpeg_error_mgr _error_mgr;
		jmp_buf _jmp_buf;
	};

	void error_callback(jpeg_common_struct* cinfo)
	{
		longjmp(reinterpret_cast<JpegErrorHandler*>(cinfo->err)->_jmp_buf, 1);
	}
}

namespace Yttrium
{
	bool read_jpeg(File& file, ImageFormat& format, Buffer& buffer)
	{
		Buffer file_buffer;
		try
		{
			if (!file.read_all(&file_buffer))
				return false;
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}

		JpegErrorHandler error_handler;
		error_handler._error_mgr.error_exit = ::error_callback;
		if (::setjmp(error_handler._jmp_buf))
			return false;

		jpeg_decompress_struct decompressor;
		decompressor.err = ::jpeg_std_error(&error_handler._error_mgr);
		::jpeg_create_decompress(&decompressor);
		if (::setjmp(error_handler._jmp_buf))
		{
			::jpeg_destroy_decompress(&decompressor);
			return false;
		}

		// TODO: Consider switching to jpeg_source_mgr.

		::jpeg_mem_src(&decompressor, &file_buffer[0], file_buffer.size());

		::jpeg_read_header(&decompressor, TRUE);

		decompressor.out_color_space = JCS_RGB;

		::jpeg_calc_output_dimensions(&decompressor);

		format.set_pixel_format(PixelFormat::Rgb, 24);
		format.set_orientation(ImageOrientation::XRightYDown);
		format.set_width(decompressor.output_width);
		format.set_height(decompressor.output_height);

		try
		{
			buffer.reset(format.frame_size());
		}
		catch (const std::bad_alloc&)
		{
			::jpeg_destroy_decompress(&decompressor);
			return false;
		}

		::jpeg_start_decompress(&decompressor);
		for (auto scanline = &buffer[0]; decompressor.output_scanline < decompressor.output_height; scanline += format.row_size())
			::jpeg_read_scanlines(&decompressor, &scanline, 1);
		::jpeg_finish_decompress(&decompressor);

		if (::setjmp(error_handler._jmp_buf))
			return false;

		::jpeg_destroy_decompress(&decompressor);

		return true;
	}
}
