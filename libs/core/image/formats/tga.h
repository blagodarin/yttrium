//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef _src_image_formats_tga_h_
#define _src_image_formats_tga_h_

#include <cstdint>

namespace Yttrium
{
	enum TgaImageDescriptor : uint8_t
	{
		tgaAlphaMask = 0x0F,

		tgaOriginMask = 0x30,
		tgaBottomLeft = 0x00,
		tgaBottomRight = 0x10,
		tgaTopLeft = 0x20,
		tgaTopRight = 0x30,

		tgaReservedMask = 0xC0,
	};

	enum TgaColorMapType : uint8_t
	{
		tgaNoColorMap,
		tgaColorMap,
	};

	enum TgaImageType : uint8_t
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
		uint8_t entry_size;
	};

	struct TgaImage
	{
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;
		uint8_t pixel_depth;
		uint8_t descriptor;
	};

	struct TgaHeader
	{
		uint8_t id_length;
		uint8_t color_map_type;
		uint8_t image_type;
		TgaColorMap color_map;
		TgaImage image;
	};

#pragma pack(pop)

	enum class TgaRlePacketHeader
	{
		PacketTypeMask = 0x80,
		RawPacket = 0x00,
		RunLengthPacket = 0x80,

		PixelCountMask = 0x7F,
	};
}

#endif
