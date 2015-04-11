#ifndef SRC_IMAGE_TGAPRIVATE_H
#define SRC_IMAGE_TGAPRIVATE_H

#include <cstdint>

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
}

#endif
