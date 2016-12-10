#include "tga.h"
#include "../formats.h"

#include <yttrium/image.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>

namespace
{
	using namespace Yttrium;

	bool can_write(const ImageFormat& format)
	{
		switch (format.pixel_format())
		{
		case PixelFormat::Gray:
			if (format.bits_per_pixel() != 8)
				return false;
			break;

		case PixelFormat::Bgr:
			if (format.bits_per_pixel() != 24)
				return false;
			break;

		case PixelFormat::Bgra:
			if (format.bits_per_pixel() != 32)
				return false;
			break;

		default:
			return false;
		}

		if (format.orientation() != ImageOrientation::XRightYDown
			&& format.orientation() != ImageOrientation::XRightYUp
			&& format.orientation() != ImageOrientation::XLeftYDown
			&& format.orientation() != ImageOrientation::XLeftYUp)
			return false;

		if (format.width() <= 0 || format.width() > UINT16_MAX)
			return false;

		if (format.height() <= 0 || format.height() > UINT16_MAX)
			return false;

		return true;
	}
}

namespace Yttrium
{
	bool read_tga_header(Reader& reader, ImageFormat& format)
	{
		TgaHeader header;

		if (!reader.read(header)
			|| header.color_map_type != tgaNoColorMap
			|| !header.image.width
			|| !header.image.height
			|| header.image.descriptor & tgaReservedMask)
			return false;

		if (header.image_type == tgaTrueColor)
		{
			const auto alpha = header.image.descriptor & tgaAlphaMask;
			if (!alpha && header.image.pixel_depth == 24)
				format.set_pixel_format(PixelFormat::Bgr, 24);
			else if (alpha == 8 && header.image.pixel_depth == 32)
				format.set_pixel_format(PixelFormat::Bgra, 32);
			else
				return false;
		}
		else if (header.image_type == tgaBlackAndWhite && header.image.pixel_depth == 8)
			format.set_pixel_format(PixelFormat::Gray, 8);
		else
			return false;

		format.set_width(header.image.width);
		format.set_height(header.image.height);

		switch (header.image.descriptor & tgaOriginMask)
		{
		case tgaBottomLeft: format.set_orientation(ImageOrientation::XRightYUp); break;
		case tgaBottomRight: format.set_orientation(ImageOrientation::XLeftYUp); break;
		case tgaTopLeft: format.set_orientation(ImageOrientation::XRightYDown); break;
		case tgaTopRight: format.set_orientation(ImageOrientation::XLeftYDown); break;
		}

		if (header.id_length)
			reader.skip(header.id_length);

		if (header.color_map.length)
			reader.skip(header.color_map.length * ((header.color_map.entry_size + 7) / 8));

		return true;
	}

	bool write_tga(Writer& writer, const ImageFormat& format, const void* data)
	{
		if (!::can_write(format))
			return false;

		TgaHeader header;

		header.id_length = 0;
		header.color_map_type = tgaNoColorMap;
		header.image_type = (format.pixel_format() == PixelFormat::Gray) ? tgaBlackAndWhite : tgaTrueColor;
		header.color_map.first_entry_index = 0;
		header.color_map.length = 0;
		header.color_map.entry_size = 0;
		header.image.x = 0;
		header.image.y = 0;
		header.image.width = format.width();
		header.image.height = format.height();
		header.image.pixel_depth = format.bits_per_pixel();
		header.image.descriptor = (format.pixel_format() == PixelFormat::Bgra) ? 8 : 0;

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
