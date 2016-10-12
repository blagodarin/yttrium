#include <yttrium/image.h>

#include <yttrium/io/reader.h>
#include <yttrium/io/writer.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/static_string.h>
#include <yttrium/utils.h>
#include "../io/writer.h"
#include "formats.h"

namespace
{
	using namespace Yttrium;

	size_t unaligned_row_size(size_t width, size_t bits_per_pixel)
	{
		return (width * bits_per_pixel + 7) / 8;
	}

	size_t aligned_row_size(size_t width, size_t bits_per_pixel, size_t row_alignment)
	{
		return (unaligned_row_size(width, bits_per_pixel) + row_alignment - 1) / row_alignment * row_alignment;
	}
}

namespace Yttrium
{
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
		_row_size = ::aligned_row_size(_width, bits_per_pixel, _row_alignment);
	}

	void ImageFormat::set_row_alignment(size_t alignment)
	{
		if (is_power_of_2(alignment))
		{
			_row_alignment = alignment;
			_row_size = ::aligned_row_size(_width, _bits_per_pixel, alignment);
		}
		// TODO: Think of a better behavior on trying to set an invalid alignment than just ignoring to change anything.
	}

	void ImageFormat::set_width(size_t width)
	{
		_width = width;
		_row_size = ::aligned_row_size(width, _bits_per_pixel, _row_alignment);
	}

	// TODO: Add a function to set width and alignment simultaneously (and more efficiently) to use in Image::set_size.

	Image::Image(const ImageFormat& format)
		: _format(format)
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
		return load(Reader(name), type);
	}

	bool Image::load(Reader&& reader, ImageType type)
	{
		if (!reader)
			return false;
		if (type == ImageType::Auto && !detect_image_type(reader, type))
			return false;
		ImageFormat format;
		Buffer buffer;
		if (!read_image(reader, type, format, buffer))
			return false;
		_format = format;
		_buffer = std::move(buffer);
		return true;
	}

	bool Image::save(const StaticString& name, ImageType type) const
	{
		if (type == ImageType::Auto)
		{
			if (name.ends_with(".tga"_s))
				type = ImageType::Tga;
	#ifndef Y_NO_PNG
			else if (name.ends_with(".png"_s))
				type = ImageType::Png;
	#endif
			else
				return false;
		}
		return save(Writer(name), type);
	}

	bool Image::save(Writer&& writer, ImageType type) const
	{
		return write_image(writer, type, _format, _buffer.data());
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

	Buffer Image::to_buffer(ImageType type)
	{
		Buffer buffer;
		return save(Writer(buffer), type) ? std::move(buffer) : Buffer();
	}

	bool operator==(const Image& lhs, const Image& rhs)
	{
		// This implementation relies on equal padding data (if any).
		return lhs._format == rhs._format
			&& lhs._buffer == rhs._buffer; // TODO: Fix the capacity-less buffer comparison.
	}
}
