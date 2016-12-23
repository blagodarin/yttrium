#include <yttrium/image.h>

#include <yttrium/static_string.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include <yttrium/utils.h>
#include "formats.h"
#include "utils.h"

#include <cassert>

namespace
{
	using namespace Yttrium;

	size_t channels_of(PixelFormat pixel_format)
	{
		switch (pixel_format)
		{
		case PixelFormat::Gray: return 1;
		case PixelFormat::GrayAlpha: return 2;
		case PixelFormat::Rgb: return 3;
		case PixelFormat::Bgr: return 3;
		case PixelFormat::Rgba: return 4;
		case PixelFormat::Bgra: return 4;
		}
		assert(false);
		return 0;
	}
}

namespace Yttrium
{
	ImageFormat::ImageFormat(size_t width, size_t height, PixelFormat pixel_format, size_t bits_per_pixel, size_t row_alignment, ImageOrientation orientation)
		: _pixel_format(pixel_format)
		, _channels(::channels_of(pixel_format))
		, _bits_per_pixel(bits_per_pixel)
		, _orientation(orientation)
		, _width(width)
		, _row_alignment(row_alignment)
		, _row_size(aligned_image_row_size(_width, _bits_per_pixel, _row_alignment))
		, _height(height)
	{
		assert(is_power_of_2(_row_alignment));
	}

	Image::Image(const ImageFormat& format)
		: _format(format)
		, _buffer(_format.frame_size())
	{
	}

	Image::Image(const ImageFormat& format, const void* data)
		: _format(format)
		, _buffer(_format.frame_size(), data)
	{
	}

	boost::optional<Image> Image::load(Reader&& reader, ImageType type)
	{
		if (!reader)
			return {};
		if (type == ImageType::Auto && !detect_image_type(reader, type))
			return {};
		Buffer buffer;
		const auto format = read_image(reader, type, buffer);
		if (!format)
			return {};
		return Image(*format, std::move(buffer));
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

	bool Image::swap_channels()
	{
		switch (_format.pixel_format())
		{
		case PixelFormat::Gray:
			return true;

		case PixelFormat::Rgb:
		case PixelFormat::Bgr:
			if (_format.bits_per_pixel() == 24)
			{
				auto scanline = static_cast<uint8_t*>(_buffer.data());
				for (size_t row = 0; row < _format.height(); ++row)
				{
					for (size_t offset = 0; offset < _format.width() * 3; offset += 3)
					{
						const auto x = scanline[offset];
						scanline[offset] = scanline[offset + 2];
						scanline[offset + 2] = x;
					}
					scanline += _format.row_size();
				}
				_format._pixel_format = (_format.pixel_format() == PixelFormat::Rgb ? PixelFormat::Bgr : PixelFormat::Rgb);
				return true;
			}
			break;

		default:
			break;
		}

		return false;
	}

	Buffer Image::to_buffer(ImageType type) const
	{
		Buffer buffer;
		return save(Writer(buffer), type) ? std::move(buffer) : Buffer();
	}
}
