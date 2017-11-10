#include "tga.h"
#include "../formats.h"

#include <yttrium/image.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>

#include <limits>

namespace
{
	bool can_write(const Yttrium::ImageFormat& format)
	{
		using Yttrium::ImageOrientation;
		using Yttrium::PixelFormat;

		switch (format.pixel_format())
		{
		case PixelFormat::Gray8:
		case PixelFormat::Bgr24:
		case PixelFormat::Bgra32:
			break;
		default:
			return false;
		}

		if (format.orientation() != ImageOrientation::XRightYDown
			&& format.orientation() != ImageOrientation::XRightYUp
			&& format.orientation() != ImageOrientation::XLeftYDown
			&& format.orientation() != ImageOrientation::XLeftYUp)
			return false;

		if (format.width() <= 0 || format.width() > std::numeric_limits<uint16_t>::max())
			return false;

		if (format.height() <= 0 || format.height() > std::numeric_limits<uint16_t>::max())
			return false;

		return true;
	}
}

namespace Yttrium
{
	std::optional<ImageFormat> read_tga_header(Reader& reader)
	{
		TgaHeader header;

		if (!reader.read(header)
			|| header.color_map_type != tgaNoColorMap
			|| !header.image.width
			|| !header.image.height
			|| header.image.descriptor & tgaReservedMask)
			return {};

		const auto orientation = [&header]
		{
			switch (header.image.descriptor & tgaOriginMask)
			{
			case tgaBottomLeft:  return ImageOrientation::XRightYUp;
			case tgaBottomRight: return ImageOrientation::XLeftYUp;
			case tgaTopRight:    return ImageOrientation::XLeftYDown;
			default:             return ImageOrientation::XRightYDown;
			}
		};

		std::optional<ImageFormat> format;

		if (header.image_type == tgaTrueColor)
		{
			const auto alpha = header.image.descriptor & tgaAlphaMask;
			if (!alpha && header.image.pixel_depth == 24)
				format.emplace(header.image.width, header.image.height, PixelFormat::Bgr24, orientation());
			else if (alpha == 8 && header.image.pixel_depth == 32)
				format.emplace(header.image.width, header.image.height, PixelFormat::Bgra32, orientation());
			else
				return {};
		}
		else if (header.image_type == tgaBlackAndWhite && header.image.pixel_depth == 8)
			format.emplace(header.image.width, header.image.height, PixelFormat::Gray8, orientation());
		else
			return {};

		if (header.id_length && !reader.skip(header.id_length))
			return {};

		if (header.color_map.length && !reader.skip(header.color_map.length * ((header.color_map.entry_size + 7u) / 8u)))
			return {};

		return format;
	}

	bool write_tga(Writer& writer, const ImageFormat& format, const void* data)
	{
		if (!::can_write(format))
			return false;

		TgaHeader header;

		header.id_length = 0;
		header.color_map_type = tgaNoColorMap;
		header.image_type = (format.pixel_format() == PixelFormat::Gray8) ? tgaBlackAndWhite : tgaTrueColor;
		header.color_map.first_entry_index = 0;
		header.color_map.length = 0;
		header.color_map.entry_size = 0;
		header.image.x = 0;
		header.image.y = 0;
		header.image.width = static_cast<uint16_t>(format.width());
		header.image.height = static_cast<uint16_t>(format.height());
		header.image.pixel_depth = static_cast<uint8_t>(format.bits_per_pixel());
		header.image.descriptor = (format.pixel_format() == PixelFormat::Bgra32) ? 8 : 0;

		switch (format.orientation())
		{
		case ImageOrientation::XRightYDown: header.image.descriptor |= tgaTopLeft;     break;
		case ImageOrientation::XRightYUp:   header.image.descriptor |= tgaBottomLeft;  break;
		case ImageOrientation::XLeftYDown:  header.image.descriptor |= tgaTopRight;    break;
		case ImageOrientation::XLeftYUp:    header.image.descriptor |= tgaBottomRight; break;
		}

		writer.reserve(sizeof header + format.frame_size());
		if (!writer.write(header))
			return false;

		auto scanline = static_cast<const uint8_t*>(data);
		for (size_t row = 0; row < format.height(); ++row)
		{
			if (!writer.write(scanline, format.row_size()))
				return false;
			scanline += format.row_size();
		}

		return true;
	}
}
