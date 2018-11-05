#include <yttrium/image.h>

#include <yttrium/storage/writer.h>
#include <yttrium/utils/numeric.h>
#include "../utils/string.h"
#include "formats.h"
#include "utils.h"

#include <cassert>

namespace
{
	using Yttrium::PixelFormat;

	constexpr std::size_t channels_of(PixelFormat format) noexcept
	{
		switch (format)
		{
		case PixelFormat::Gray8: return 1;
		case PixelFormat::GrayAlpha16: return 2;
		case PixelFormat::Rgb24: return 3;
		case PixelFormat::Bgr24: return 3;
		case PixelFormat::Rgba32: return 4;
		case PixelFormat::Bgra32: return 4;
		}
		return 0;
	}
}

namespace Yttrium
{
	ImageFormat::ImageFormat(size_t width, size_t height, PixelFormat pixel_format, size_t row_alignment, ImageOrientation orientation)
		: _pixel_format{pixel_format}
		, _channels{::channels_of(_pixel_format)}
		, _orientation{orientation}
		, _width{width}
		, _row_alignment{row_alignment}
		, _row_size{aligned_image_row_size(_width, pixel_size(_pixel_format) * 8, _row_alignment)}
		, _height{height}
	{
		assert(is_power_of_2(_row_alignment));
	}

	std::optional<Image> Image::load(const Source& source, ImageType type)
	{
		if (type == ImageType::Auto && !detect_image_type(source, type))
			return {};
		Buffer buffer;
		const auto format = read_image(source, type, buffer);
		if (!format)
			return {};
		return Image{*format, std::move(buffer)};
	}

	Image::Image(const ImageFormat& format)
		: _format{format}
		, _buffer{_format.frame_size()}
	{
	}

	Image::Image(const ImageFormat& format, const void* data)
		: _format{format}
		, _buffer{_format.frame_size(), data}
	{
	}

	bool Image::save(const std::string& path, ImageType type) const
	{
		if (type == ImageType::Auto)
		{
			if (ends_with(path, ".tga"))
				type = ImageType::Tga;
#if Y_USE_PNG
			else if (ends_with(path, ".png"))
				type = ImageType::Png;
#endif
			else
				return false;
		}
		return save(Writer{path}, type);
	}

	bool Image::save(Writer&& writer, ImageType type) const
	{
		return write_image(writer, type, _format, _buffer.data());
	}

	bool Image::swap_channels() noexcept
	{
		switch (_format.pixel_format())
		{
		case PixelFormat::Gray8:
			return true;

		case PixelFormat::Rgb24:
		case PixelFormat::Bgr24:
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
			}
			_format._pixel_format = _format.pixel_format() == PixelFormat::Rgb24 ? PixelFormat::Bgr24 : PixelFormat::Rgb24;
			return true;

		default:
			break;
		}

		return false;
	}

	Buffer Image::to_buffer(ImageType type) const
	{
		Buffer buffer;
		return save(Writer{buffer}, type) ? std::move(buffer) : Buffer{};
	}
}
