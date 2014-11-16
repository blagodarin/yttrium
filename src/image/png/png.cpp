#include "png.h"

namespace Yttrium
{

namespace
{

void _png_write_callback(png_structp png_ptr, png_bytep data, png_size_t length)
{
	reinterpret_cast<File*>(png_get_io_ptr(png_ptr))->write(data, length);
}

void _png_flush_callback(png_structp png_ptr)
{
	reinterpret_cast<File*>(png_get_io_ptr(png_ptr))->flush();
}

} // namespace

PngWriter::PngWriter(Allocator* allocator)
	: ImageWriter(allocator)
	, _png(nullptr)
	, _info(nullptr)
{
}

PngWriter::~PngWriter()
{
	if (_png)
	{
		if (_info)
			png_destroy_write_struct(&_png, &_info);
		else
			png_destroy_write_struct(&_png, nullptr);
	}
}

bool PngWriter::open()
{
	_png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

	if (_png)
	{
		_info = png_create_info_struct(_png);
		if (_info)
		{
			png_set_write_fn(_png, &_file, _png_write_callback, _png_flush_callback);
			return true;
			//png_destroy_write_struct(&_png, &_info);
		}
		else
		{
			png_destroy_write_struct(&_png, nullptr);
		}
	}

	return false;
}

bool PngWriter::set_format(const ImageFormat &format)
{
	switch (format.pixel_format())
	{
	case PixelFormat::Gray:

		if (format.bits_per_pixel() != 8 && format.bits_per_pixel() != 16)
			return false;
		break;

	case PixelFormat::GrayAlpha:
	case PixelFormat::AlphaGray:

		if (format.bits_per_pixel() != 16 && format.bits_per_pixel() != 32)
			return false;
		break;

	case PixelFormat::Rgb:
	case PixelFormat::Bgr:

		if (format.bits_per_pixel() != 24 && format.bits_per_pixel() != 48)
			return false;
		break;

	case PixelFormat::Rgba:
	case PixelFormat::Bgra:
	case PixelFormat::Argb:
	case PixelFormat::Abgr:

		if (format.bits_per_pixel() != 32 && format.bits_per_pixel() != 64)
			return false;
		break;

	default:

		return false;
	}

	if (format.orientation() != ImageOrientation::XRightYDown
		&& format.orientation() != ImageOrientation::XRightYUp)
	{
		return false;
	}

	if (format.width() <= 0 || format.width() > UINT32_MAX)
		return false;

	if (format.height() <= 0 || format.height() > UINT32_MAX)
		return false;

	return true;
}

bool PngWriter::write(const void* buffer)
{
	int color_type = 0;
	int transforms = 0;

	if (_format.pixel_format() == PixelFormat::AlphaGray
		|| _format.pixel_format() == PixelFormat::Argb
		|| _format.pixel_format() == PixelFormat::Abgr)
	{
		transforms |= PNG_TRANSFORM_SWAP_ALPHA;
	}

	if (_format.pixel_format() == PixelFormat::Bgr
		|| _format.pixel_format() == PixelFormat::Bgra
		|| _format.pixel_format() == PixelFormat::Abgr)
	{
		transforms |= PNG_TRANSFORM_BGR;
	}

	// TODO: Don't forget not to swap bytes on a big endian target platform if one appears.

	switch (_format.pixel_format())
	{
	case PixelFormat::Gray:

		if (_format.bits_per_pixel() > 8)
			transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
		color_type = PNG_COLOR_TYPE_GRAY;
		break;

	case PixelFormat::GrayAlpha:
	case PixelFormat::AlphaGray:

		if (_format.bits_per_pixel() > 16)
			transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
		color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
		break;

	case PixelFormat::Rgb:
	case PixelFormat::Bgr:

		if (_format.bits_per_pixel() > 24)
			transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
		color_type = PNG_COLOR_TYPE_RGB;
		break;

	case PixelFormat::Rgba:
	case PixelFormat::Bgra:
	case PixelFormat::Argb:
	case PixelFormat::Abgr:

		if (_format.bits_per_pixel() > 32)
			transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
		color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		break;
	}

	png_bytepp rows = _allocator->allocate<png_bytep>(_format.height());

	if (setjmp(png_jmpbuf(_png)))
	{
		_allocator->deallocate(rows);
		return false;
	}

	png_set_compression_level(_png, 0);
	png_set_IHDR(_png, _info, _format.width(), _format.height(), _format.bits_per_channel(),
		color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	size_t j = 0;

	if (_format.orientation() == ImageOrientation::XRightYDown)
	{
		for (size_t i = 0; i < _format.height(); i++, j += _format.row_size())
			rows[i] = const_cast<png_bytep>(static_cast<png_const_bytep>(buffer) + j);
	}
	else
	{
		for (size_t i = _format.height(); i > 0; i--, j += _format.row_size())
			rows[i - 1] = const_cast<png_bytep>(static_cast<png_const_bytep>(buffer) + j);
	}

	png_set_rows(_png, _info, rows);
	png_write_png(_png, _info, transforms, nullptr);

	_allocator->deallocate(rows);

	return true;
}

} // namespace Yttrium
