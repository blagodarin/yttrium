#include "image.h"

#include <yttrium/package.h>
#include <yttrium/utils.h>

#include "../base/allocatable.h"
#include "dds.h"
#include "tga.h"

#ifndef Y_NO_JPEG
	#include "jpeg/jpeg.h"
#endif

#ifndef Y_NO_PNG
	#include "png/png.h"
#endif

namespace Yttrium
{

namespace
{

size_t unaligned_row_size(size_t width, size_t bits_per_pixel)
{
	return (width * bits_per_pixel + 7) / 8;
}

size_t aligned_row_size(size_t width, size_t bits_per_pixel, size_t row_alignment)
{
	return (unaligned_row_size(width, bits_per_pixel) + row_alignment - 1) / row_alignment * row_alignment;
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

void ImageFormat::set_row_alignment(size_t alignment)
{
	if (Y_LIKELY(is_power_of_2(alignment)))
	{
		_row_alignment = alignment;
		_row_size = aligned_row_size(_width, _bits_per_pixel, alignment);
	}
	// TODO: Think of a better behavior on trying to set an invalid alignment than just ignoring to change anything.
}

void ImageFormat::set_width(size_t width)
{
	_width = width;
	_row_size = aligned_row_size(width, _bits_per_pixel, _row_alignment);
}

// TODO: Add a function to set width and alignment simultaneously (and more efficiently) to use in Image::set_size.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImageReader::ImageReader(Allocator *allocator)
	: _allocator(allocator)
{
}

ImageReader::~ImageReader()
{
}

ImageWriter::ImageWriter(Allocator *allocator)
	: _allocator(allocator)
{
}

ImageWriter::~ImageWriter()
{
}

bool ImageWriter::open()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Image::Image(const ImageFormat &format, Allocator *allocator)
	: _format(format)
	, _buffer(_format.frame_size(), allocator)
{
}

bool Image::intensity_to_bgra()
{
	if (Y_UNLIKELY(_format._pixel_format != PixelFormat::Gray || _format._bits_per_pixel != 8))
		return false;

	size_t old_row_size = _format._row_size;

	_format._pixel_format = PixelFormat::Bgra;
	_format._channels = 4;
	_format._bits_per_pixel = 32;
	_format._row_alignment = 4;
	_format._row_size = _format._width * 4;

	Buffer output_buffer(_format.frame_size(), _buffer.allocator());

	const uint8_t *src = static_cast<const uint8_t *>(_buffer.const_data());
	uint8_t *dst = static_cast<uint8_t *>(output_buffer.data());

	for (size_t i = 0; i < _format._height; ++i)
	{
		for (size_t j = 0; j < _format._width; ++j)
		{
			*dst++ = *src;
			*dst++ = *src;
			*dst++ = *src;
			*dst++ = *src++;
		}
		src += old_row_size - _format._width;
	}

	_buffer.swap(&output_buffer);

	return true;
}

bool Image::load(const StaticString &name, ImageType type)
{
	if (Y_LIKELY(type == ImageType::Auto))
	{
		StaticString extension = name.file_extension();
		if (extension == ".tga")
			type = ImageType::Tga;
		else if (extension == ".dds")
			type = ImageType::Dds;
#ifndef Y_NO_JPEG
		else if (extension == ".jpeg" || extension == ".jpg")
			type = ImageType::Jpeg;
#endif
		else
			return false;
	}

	Allocatable<ImageReader> reader(_buffer.allocator());

	switch (type)
	{
	case ImageType::Tga:  reader.reset<TgaReader>(); break;
#ifndef Y_NO_JPEG
	case ImageType::Jpeg: reader.reset<JpegReader>(); break;
#endif
	case ImageType::Dds:  reader.reset<DdsReader>(); break;
	default:              return false;
	}

	if (Y_UNLIKELY(!reader->_file.open(name, reader.allocator())))
		return false;

	if (Y_UNLIKELY(!reader->open()))
		return false;

	_format = reader->_format;
	_buffer.resize(_format.frame_size());
	return reader->read(_buffer.data());
}

bool Image::save(const StaticString &name, ImageType type) const
{
	if (type == ImageType::Auto)
	{
		StaticString extension = name.file_extension();
		if (extension == ".tga")
			type = ImageType::Tga;
#ifndef Y_NO_PNG
		else if (extension == ".png")
			type = ImageType::Png;
#endif
		else
			return false;
	}

	Allocatable<ImageWriter> writer(_buffer.allocator());

	switch (type)
	{
	case ImageType::Tga: writer.reset<TgaWriter>(); break;
#ifndef Y_NO_PNG
	case ImageType::Png: writer.reset<PngWriter>(); break;
#endif
	default:             return false;
	}

	if (Y_UNLIKELY(!writer->_file.open(name, File::Write | File::Truncate, writer.allocator())))
		return false;

	if (Y_UNLIKELY(!writer->open() || !writer->set_format(_format)))
		return false;

	writer->_format = _format;
	return writer->write(_buffer.data());
}

void Image::set_format(const ImageFormat &format)
{
	_format = format;
	_buffer.resize(_format.frame_size());
}

void Image::set_size(size_t width, size_t height, size_t row_alignment)
{
	_format.set_height(height);
	_format.set_width(width);
	if (row_alignment)
		_format.set_row_alignment(row_alignment);
	_buffer.resize(_format.frame_size());
}

bool Image::swap_channels()
{
	switch (_format._pixel_format)
	{
	case PixelFormat::Gray:

		return true;

	case PixelFormat::Rgb:
	case PixelFormat::Bgr:

		if (Y_LIKELY(_format._bits_per_pixel == 24))
		{
			uint8_t *scanline = static_cast<uint8_t *>(_buffer.data());

			for (size_t row = 0; row < _format._height; ++row)
			{
				for (size_t offset = 0; offset < _format._width * 3; offset += 3)
				{
					uint8_t x = scanline[offset];
					scanline[offset] = scanline[offset + 2];
					scanline[offset + 2] = x;
				}
				scanline += _format.row_size();
			}

			_format._pixel_format = (_format._pixel_format == PixelFormat::Rgb ? PixelFormat::Bgr : PixelFormat::Rgb);

			return true;
		}
		break;

	default:

		break;
	}

	return false;
}

bool Image::operator ==(const Image &image) const
{
	// This implementation relies on equal padding data (if any).

	return _format == image._format
		&& _buffer == image._buffer; // TODO: Fix the capacity-less buffer comparison.
}

} // namespace Yttrium
