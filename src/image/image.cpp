#include "image.h"

#include <yttrium/package.h>
#include <yttrium/pointer.h>
#include <yttrium/utils.h>
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
	}

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
		if (is_power_of_2(alignment))
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

	bool ImageWriter::open()
	{
		return true;
	}

	Image::Image(Allocator& allocator)
		: _allocator(allocator)
	{
	}

	Image::Image(const ImageFormat& format, Allocator& allocator)
		: _allocator(allocator)
		, _format(format)
		, _buffer(_format.frame_size())
	{
	}

	bool Image::intensity_to_bgra()
	{
		if (_format._pixel_format != PixelFormat::Gray || _format._bits_per_pixel != 8)
			return false;

		const auto old_row_size = _format._row_size;

		_format._pixel_format = PixelFormat::Bgra;
		_format._channels = 4;
		_format._bits_per_pixel = 32;
		_format._row_alignment = 4;
		_format._row_size = _format._width * 4;

		Buffer output_buffer(_format.frame_size());

		const auto* src = &_buffer[0];
		auto* dst = &output_buffer[0];

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

		_buffer = std::move(output_buffer);

		return true;
	}

	bool Image::load(const StaticString& name, ImageType type)
	{
		if (type == ImageType::Auto)
		{
			const StaticString& extension = name.file_name_extension();
			if (extension == ".tga"_s)
				type = ImageType::Tga;
			else if (extension == ".dds"_s)
				type = ImageType::Dds;
	#ifndef Y_NO_JPEG
			else if (extension == ".jpeg"_s || extension == ".jpg"_s)
				type = ImageType::Jpeg;
	#endif
			else
				return false;
		}

		Pointer<ImageReader> reader;
		switch (type)
		{
		case ImageType::Tga:  reader = make_pointer<TgaReader>(_allocator, name, &_allocator); break;
	#ifndef Y_NO_JPEG
		case ImageType::Jpeg: reader = make_pointer<JpegReader>(_allocator, name, &_allocator); break;
	#endif
		case ImageType::Dds:  reader = make_pointer<DdsReader>(_allocator, name, &_allocator); break;
		default:              return false;
		}

		if (!reader->_file || !reader->open())
			return false;

		_format = reader->_format;
		_buffer.reset(_format.frame_size());
		return reader->read(_buffer.data());
	}

	bool Image::save(const StaticString& name, ImageType type) const
	{
		if (type == ImageType::Auto)
		{
			const StaticString& extension = name.file_name_extension();
			if (extension == ".tga"_s)
				type = ImageType::Tga;
	#ifndef Y_NO_PNG
			else if (extension == ".png"_s)
				type = ImageType::Png;
	#endif
			else
				return false;
		}

		Pointer<ImageWriter> writer;
		switch (type)
		{
		case ImageType::Tga: writer = make_pointer<TgaWriter>(_allocator, name, &_allocator); break;
	#ifndef Y_NO_PNG
		case ImageType::Png: writer = make_pointer<PngWriter>(_allocator, name, &_allocator); break;
	#endif
		default:             return false;
		}

		if (!writer->_file || !writer->open() || !writer->set_format(_format))
			return false;

		writer->_format = _format;
		return writer->write(_buffer.data());
	}

	void Image::set_format(const ImageFormat& format)
	{
		_format = format;
		_buffer.reset(_format.frame_size());
	}

	void Image::set_size(size_t width, size_t height, size_t row_alignment)
	{
		_format.set_height(height);
		_format.set_width(width);
		if (row_alignment)
			_format.set_row_alignment(row_alignment);
		_buffer.reset(_format.frame_size());
	}

	bool Image::swap_channels()
	{
		switch (_format._pixel_format)
		{
		case PixelFormat::Gray:

			return true;

		case PixelFormat::Rgb:
		case PixelFormat::Bgr:

			if (_format._bits_per_pixel == 24)
			{
				auto* scanline = &_buffer[0];

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

	bool operator==(const Image& lhs, const Image& rhs)
	{
		// This implementation relies on equal padding data (if any).
		return lhs._format == rhs._format
			&& lhs._buffer == rhs._buffer; // TODO: Fix the capacity-less buffer comparison.
	}
}
