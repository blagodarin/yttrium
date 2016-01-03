#include "jpeg.h"

// TODO: Substitute JpegLib's memory manager.

namespace Yttrium
{
	JpegReader::JpegReader(const StaticString& name, Allocator* allocator)
		: ImageReader(name, allocator)
	{
		_decompressor.err = jpeg_std_error(&_error_handler.pub);
		_error_handler.pub.error_exit = error_callback;
	}

	JpegReader::~JpegReader()
	{
		if (setjmp(_error_handler.setjmp_buffer))
			return;

		jpeg_destroy_decompress(&_decompressor);
	}

	bool JpegReader::open()
	{
		if (setjmp(_error_handler.setjmp_buffer))
			return false;

		jpeg_create_decompress(&_decompressor);

		// TODO: Consider switching to jpeg_source_mgr.

		if (!_file.read_all(&_buffer))
			return false;

		jpeg_mem_src(&_decompressor, &_buffer[0], _buffer.size());

		jpeg_read_header(&_decompressor, TRUE);

		_decompressor.out_color_space = JCS_RGB;

		jpeg_calc_output_dimensions(&_decompressor);

		_format.set_pixel_format(PixelFormat::Rgb, 24);
		_format.set_orientation(ImageOrientation::XRightYDown);
		_format.set_width(_decompressor.output_width);
		_format.set_height(_decompressor.output_height);

		return true;
	}

	bool JpegReader::read(void* buffer)
	{
		if (setjmp(_error_handler.setjmp_buffer))
			return false;

		jpeg_start_decompress(&_decompressor);

		for (auto scanline = static_cast<unsigned char*>(buffer);
			_decompressor.output_scanline < _decompressor.output_height;
			scanline += _format.row_size())
		{
			jpeg_read_scanlines(&_decompressor, &scanline, 1);
		}

		jpeg_finish_decompress(&_decompressor);

		return true;
	}

	void JpegReader::error_callback(jpeg_common_struct* cinfo)
	{
		JpegErrorHandler* error_handler = (JpegErrorHandler*)cinfo->err;
		//(*cinfo->err->output_message)(cinfo); // TODO: Do some error notification.
		longjmp(error_handler->setjmp_buffer, 1);
	}
}
