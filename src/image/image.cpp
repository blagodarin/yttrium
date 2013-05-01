#include "image.h"

#include <Yttrium/package.h>

#include "jpeg.h"
#include "png.h"
#include "tga.h"

namespace Yttrium
{

namespace
{

size_t aligned_row_size(size_t width, size_t bits_per_pixel, size_t row_alignment)
{
	size_t row_size = (width * bits_per_pixel + 7) / 8;
	return (row_size + row_alignment - 1) / row_alignment * row_alignment;
}

} // namespace

ImageFormat::ImageFormat()
	: _pixel_format(PixelFormat::Gray)
	, _channels(1)
	, _bits_per_pixel(1)
	, _orientation(ImageOrientation::XRightYDown)
	, _width(0)
	, _row_alignment(1)
	, _row_size(0)
	, _height(0)
{
}

void ImageFormat::set_pixel_format(PixelFormat pixel_format, size_t bits_per_pixel)
{
	_pixel_format = pixel_format;

	switch (pixel_format)
	{
	case PixelFormat::Gray:      _channels = 1; break;
	case PixelFormat::GrayAlpha: _channels = 2; break;
	case PixelFormat::AlphaGray: _channels = 2; break;
	case PixelFormat::Rgb:       _channels = 3; break;
	case PixelFormat::Bgr:       _channels = 3; break;
	case PixelFormat::Rgba:      _channels = 4; break;
	case PixelFormat::Bgra:      _channels = 4; break;
	case PixelFormat::Argb:      _channels = 4; break;
	case PixelFormat::Abgr:      _channels = 4; break;
	}

	_bits_per_pixel = bits_per_pixel;
	_row_size = aligned_row_size(_width, bits_per_pixel, _row_alignment);
}

void ImageFormat::set_row_alignment(size_t row_alignment)
{
	Y_ASSERT(row_alignment >= 1);

	_row_alignment = row_alignment;
	_row_size = aligned_row_size(_width, _bits_per_pixel, row_alignment);
}

void ImageFormat::set_width(size_t width)
{
	_width = width;
	_row_size = aligned_row_size(width, _bits_per_pixel, _row_alignment);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImageReader::ImageReader(const ImageReader &reader)
	: _private(Private::copy(reader._private))
{
}

void ImageReader::close()
{
	Private::release(&_private);
}

ImageFormat ImageReader::format() const
{
	return _private ? _private->_format : ImageFormat();
}

bool ImageReader::open(const StaticString &name, ImageType type, Allocator *allocator)
{
	close();

	switch (type)
	{
	case ImageType::Tga:

		_private = Y_NEW(allocator, TgaReader)(allocator);
		break;

	default:

		{
			StaticString extension = name.file_extension();

			if (extension == ".tga")
			{
				_private = Y_NEW(allocator, TgaReader)(allocator);
			}
			else if (extension == ".jpeg" || extension == ".jpg")
			{
				_private = Y_NEW(allocator, JpegReader)(allocator);
			}
		}
		break;
	}

	if (_private)
	{
		if (_private->_file.open(name, allocator)
			&& _private->open())
		{
			return true;
		}

		close();
	}

	return false;
}

bool ImageReader::read(void *buffer)
{
	bool result = false;

	if (Y_LIKELY(_private && !_private->_is_used))
	{
		result = _private->read(buffer);
		_private->_is_used = true;
	}

	return result;
}

ImageType ImageReader::type() const
{
	return _private ? _private->_type : ImageType::Auto;
}

ImageReader &ImageReader::operator =(const ImageReader &reader)
{
	Private::assign(&_private, reader._private);

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImageWriter::ImageWriter(const ImageWriter &reader)
	: _private(Private::copy(reader._private))
{
}

void ImageWriter::close()
{
	Private::release(&_private);
}

bool ImageWriter::open(const StaticString &name, ImageType type, Allocator *allocator)
{
	close();

	switch (type)
	{
	case ImageType::Tga:

		_private = Y_NEW(allocator, TgaWriter)(allocator);
		break;

	case ImageType::Png:

		_private = Y_NEW(allocator, PngWriter)(allocator);
		break;

	default:

		{
			StaticString extension = name.file_extension();

			if (extension == ".tga")
			{
				_private = Y_NEW(allocator, TgaWriter)(allocator);
			}
			else if (extension == ".png")
			{
				_private = Y_NEW(allocator, PngWriter)(allocator);
			}
		}
		break;
	}

	if (_private)
	{
		if (_private->_file.open(name, File::Write | File::Truncate)
			&& _private->open())
		{
			return true;
		}

		close();
	}

	return false;
}

ImageFormatFlags ImageWriter::set_format(const ImageFormat &format)
{
	if (!_private || _private->_is_ready || _private->_is_used)
		return ImageFormat::AllFlags;

	ImageFormatFlags result = _private->set_format(format);

	if (result & ImageFormat::PixelFormatFlag)
		result |= ImageFormat::BitsPerPixelFlag;

	if (!result)
	{
		_private->_is_ready = true;
		_private->_format = format;
	}

	return result;
}

bool ImageWriter::write(const void *buffer)
{
	bool result = false;

	if (_private && _private->_is_ready && !_private->_is_used)
	{
		result = _private->write(buffer);
		_private->_is_used = true;
	}

	return result;
}

ImageWriter &ImageWriter::operator =(const ImageWriter &writer)
{
	Private::assign(&_private, writer._private);

	return *this;
}

bool ImageWriter::Private::open()
{
	return true;
}

} // namespace Yttrium
