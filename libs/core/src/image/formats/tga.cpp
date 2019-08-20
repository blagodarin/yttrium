//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <yttrium/image/image.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include "../formats.h"

#include <cstring>
#include <limits>

namespace
{
	enum class TgaColorMapType : uint8_t
	{
		None = 0,
		ColorMap = 1,
	};

	enum class TgaImageType : uint8_t
	{
		None = 0,
		ColorMapped = 1,
		TrueColor = 2,
		BlackAndWhite = 3,
		ColorMappedRle = 9,
		TrueColorRle = 10,
		BlackAndWhiteRle = 11,
	};

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

#pragma pack(push, 1)

	struct TgaHeader
	{
		uint8_t id_length;
		TgaColorMapType color_map_type;
		TgaImageType image_type;
		struct
		{
			// cppcheck-suppress unusedStructMember
			uint16_t first_entry_index;
			uint16_t length;
			uint8_t entry_size;
		} color_map;
		struct
		{
			// cppcheck-suppress unusedStructMember
			uint16_t x;
			// cppcheck-suppress unusedStructMember
			uint16_t y;
			uint16_t width;
			uint16_t height;
			uint8_t pixel_depth;
			uint8_t descriptor;
		} image;
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

namespace Yt
{
	bool read_tga_header(Reader& reader, ImageInfo& info)
	{
		TgaHeader header;
		if (!reader.read(header)
			|| header.color_map_type != TgaColorMapType::None
			|| !header.image.width
			|| !header.image.height
			|| header.image.descriptor & tgaReservedMask)
			return false;

		PixelFormat pixel_format;
		if (header.image_type == TgaImageType::TrueColor)
		{
			const auto alpha = header.image.descriptor & tgaAlphaMask;
			if (!alpha && header.image.pixel_depth == 24)
				pixel_format = PixelFormat::Bgr24;
			else if (alpha == 8 && header.image.pixel_depth == 32)
				pixel_format = PixelFormat::Bgra32;
			else
				return false;
		}
		else if (header.image_type == TgaImageType::BlackAndWhite && header.image.pixel_depth == 8)
			pixel_format = PixelFormat::Gray8;
		else
			return false;

		ImageOrientation orientation;
		if (const auto origin = header.image.descriptor & tgaOriginMask; origin == tgaBottomLeft)
			orientation = ImageOrientation::XRightYUp;
		else if (origin == tgaTopLeft)
			orientation = ImageOrientation::XRightYDown;
		else
			return false;

		if (header.id_length && !reader.skip(header.id_length))
			return false;

		if (header.color_map.length && !reader.skip(header.color_map.length * ((header.color_map.entry_size + 7u) / 8u)))
			return false;

		info = { header.image.width, header.image.height, pixel_format, orientation };
		return true;
	}

	bool write_tga(Writer& writer, const ImageInfo& info, const void* data)
	{
		if (!info.width() || info.width() > std::numeric_limits<uint16_t>::max())
			return false;

		if (!info.height() || info.height() > std::numeric_limits<uint16_t>::max())
			return false;

		TgaHeader header;
		std::memset(&header, 0, sizeof header);
		header.color_map_type = TgaColorMapType::None;
		header.image.width = static_cast<uint16_t>(info.width());
		header.image.height = static_cast<uint16_t>(info.height());

		switch (info.pixel_format())
		{
		case PixelFormat::Gray8:
			header.image_type = TgaImageType::BlackAndWhite;
			header.image.pixel_depth = 8;
			break;
		case PixelFormat::Bgr24:
			header.image_type = TgaImageType::TrueColor;
			header.image.pixel_depth = 24;
			break;
		case PixelFormat::Bgra32:
			header.image_type = TgaImageType::TrueColor;
			header.image.pixel_depth = 32;
			header.image.descriptor = 8;
			break;
		default:
			return false;
		}

		switch (info.orientation())
		{
		case ImageOrientation::XRightYDown:
			header.image.descriptor |= tgaTopLeft;
			break;
		case ImageOrientation::XRightYUp:
			header.image.descriptor |= tgaBottomLeft;
			break;
		default:
			return false;
		}

		if (!writer.try_reserve(sizeof header + info.frame_size()) || !writer.write(header))
			return false;

		auto scanline = static_cast<const uint8_t*>(data);
		for (size_t row = 0; row < info.height(); ++row)
		{
			if (!writer.write(scanline, info.stride()))
				return false;
			scanline += info.stride();
		}

		return true;
	}
}
