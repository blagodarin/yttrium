#include "tga.h"

namespace Yttrium
{

enum TgaImageDescriptor: uint8_t
{
	tgaAlphaMask = 0x0F,

	tgaOriginMask  = 0x30,
	tgaBottomLeft  = 0x00,
	tgaBottomRight = 0x10,
	tgaTopLeft     = 0x20,
	tgaTopRight    = 0x30,

	tgaReservedMask = 0xC0,
};

enum TgaColorMapType: uint8_t
{
	tgaNoColorMap,
	tgaColorMap,
};

enum TgaImageType: uint8_t
{
	tgaNone,

	tgaColorMapped,
	tgaTrueColor,
	tgaBlackAndWhite,

	tgaColorMappedRle = 9,
	tgaTrueColorRle,
	tgaBlackAndWhiteRle,
};

#pragma pack(push, 1)

struct TgaColorMap
{
	uint16_t first_entry_index;
	uint16_t length;
	uint8_t  entry_size;
};

struct TgaImage
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint8_t  pixel_depth;
	uint8_t  descriptor;
};

struct TgaHeader
{
	uint8_t     id_length;
	uint8_t     color_map_type;
	uint8_t     image_type;
	TgaColorMap color_map;
	TgaImage    image;
};

#pragma pack(pop)

enum class TgaRlePacketHeader
{
	PacketTypeMask  = 0x80,
	RawPacket       = 0x00,
	RunLengthPacket = 0x80,

	PixelCountMask = 0x7F,
};

bool TgaReader::open()
{
	TgaHeader header;

	do
	{
		if (!_file.read(&header)
			|| header.color_map_type != tgaNoColorMap
			|| !header.image.width
			|| !header.image.height
			|| header.image.descriptor & tgaReservedMask)
		{
			break;
		}

		if (header.image_type == tgaTrueColor)
		{
			uint_fast8_t alpha = header.image.descriptor & tgaAlphaMask;

			if (!alpha && header.image.pixel_depth == 24)
			{
				_format.set_pixel_format(PixelFormat::Bgr, 24);
			}
			else if (alpha == 8 && header.image.pixel_depth == 32)
			{
				_format.set_pixel_format(PixelFormat::Bgra, 32);
			}
			else
			{
				break;
			}
		}
		else if (header.image_type == tgaBlackAndWhite && header.image.pixel_depth == 8)
		{
			_format.set_pixel_format(PixelFormat::Gray, 8);
		}
		else
		{
			break;
		}

		_format.set_width(header.image.width);
		_format.set_height(header.image.height);

		switch (header.image.descriptor & tgaOriginMask)
		{
		case tgaBottomLeft:  _format.set_orientation(ImageOrientation::XRightYUp);   break;
		case tgaBottomRight: _format.set_orientation(ImageOrientation::XLeftYUp);    break;
		case tgaTopLeft:     _format.set_orientation(ImageOrientation::XRightYDown); break;
		case tgaTopRight:    _format.set_orientation(ImageOrientation::XLeftYDown);  break;
		}

		if (header.id_length)
		{
			_file.skip(header.id_length);
		}

		if (header.color_map.length)
		{
			_file.skip(header.color_map.length * ((header.color_map.entry_size + 7) / 8));
		}

		return true;
	} while (false);

	return false;
}

bool TgaReader::read(void *buffer, size_t frame_size)
{
	return _file.read(buffer, frame_size) == frame_size;
}

ImageFormatFlags TgaWriter::set_format(const ImageFormat &format)
{
	ImageFormatFlags result = 0;

	switch (format.pixel_format())
	{
	case PixelFormat::Gray:

		if (format.bits_per_pixel() != 8)
			result |= ImageFormat::BitsPerPixelFlag;
		break;

	case PixelFormat::Bgr:

		if (format.bits_per_pixel() != 24)
			result |= ImageFormat::BitsPerPixelFlag;
		break;

	case PixelFormat::Bgra:

		if (format.bits_per_pixel() != 32)
			result |= ImageFormat::BitsPerPixelFlag;
		break;

	default:

		result |= ImageFormat::PixelFormatFlag;
	}

	if (format.orientation() != ImageOrientation::XRightYDown
		&& format.orientation() != ImageOrientation::XRightYUp
		&& format.orientation() != ImageOrientation::XLeftYDown
		&& format.orientation() != ImageOrientation::XLeftYUp)
	{
		result |= ImageFormat::OrientationFlag;
	}

	if (!format.width() || format.width() > UINT16_MAX)
	{
		result |= ImageFormat::WidthFlag;
	}

	if (!format.height() || format.height() > UINT16_MAX)
	{
		result |= ImageFormat::HeightFlag;
	}

	return result;
}

bool TgaWriter::write(const void *buffer, size_t frame_size)
{
	TgaHeader header;

	header.id_length = 0;
	header.color_map_type = tgaNoColorMap;
	header.image_type = (_format.pixel_format() == PixelFormat::Gray) ? tgaBlackAndWhite : tgaTrueColor;
	header.color_map.first_entry_index = 0;
	header.color_map.length = 0;
	header.color_map.entry_size = 0;
	header.image.x = 0;
	header.image.y = 0;
	header.image.width = _format.width();
	header.image.height = _format.height();
	header.image.pixel_depth = _format.bits_per_pixel();
	header.image.descriptor = (_format.pixel_format() == PixelFormat::Bgra) ? 8 : 0;

	switch (_format.orientation())
	{
	case ImageOrientation::XRightYDown: header.image.descriptor |= tgaTopLeft;     break;
	case ImageOrientation::XRightYUp:   header.image.descriptor |= tgaBottomLeft;  break;
	case ImageOrientation::XLeftYDown:  header.image.descriptor |= tgaTopRight;    break;
	case ImageOrientation::XLeftYUp:    header.image.descriptor |= tgaBottomRight; break;
	}

	_file.write(header);
	_file.write(buffer, frame_size);
	_file.truncate();

	return true;
}

} // namespace Yttrium
