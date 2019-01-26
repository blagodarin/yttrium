//
// Copyright 2019 Sergei Blagodarin
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

#include "tga.h"

#include <yttrium/image.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include "../formats.h"

#include <limits>

namespace
{
	bool can_write(const Yttrium::ImageInfo& info) noexcept
	{
		using Yttrium::ImageOrientation;
		using Yttrium::PixelFormat;

		switch (info.pixel_format())
		{
		case PixelFormat::Gray8:
		case PixelFormat::Bgr24:
		case PixelFormat::Bgra32:
			break;
		default:
			return false;
		}

		if (info.orientation() != ImageOrientation::XRightYDown
			&& info.orientation() != ImageOrientation::XRightYUp
			&& info.orientation() != ImageOrientation::XLeftYDown
			&& info.orientation() != ImageOrientation::XLeftYUp)
			return false;

		if (info.width() <= 0 || info.width() > std::numeric_limits<std::uint16_t>::max())
			return false;

		if (info.height() <= 0 || info.height() > std::numeric_limits<std::uint16_t>::max())
			return false;

		return true;
	}
}

namespace Yttrium
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

		if (header.id_length && !reader.skip(header.id_length))
			return false;

		if (header.color_map.length && !reader.skip(header.color_map.length * ((header.color_map.entry_size + 7u) / 8u)))
			return false;

		const auto orientation = [&header] {
			switch (header.image.descriptor & tgaOriginMask)
			{
			case tgaBottomLeft: return ImageOrientation::XRightYUp;
			case tgaBottomRight: return ImageOrientation::XLeftYUp;
			case tgaTopRight: return ImageOrientation::XLeftYDown;
			default: return ImageOrientation::XRightYDown;
			}
		};

		info = { header.image.width, header.image.height, pixel_format, orientation() };
		return true;
	}

	bool write_tga(Writer& writer, const ImageInfo& info, const void* data)
	{
		if (!::can_write(info))
			return false;

		TgaHeader header;
		header.id_length = 0;
		header.color_map_type = TgaColorMapType::None;
		header.image_type = (info.pixel_format() == PixelFormat::Gray8) ? TgaImageType::BlackAndWhite : TgaImageType::TrueColor;
		header.color_map.first_entry_index = 0;
		header.color_map.length = 0;
		header.color_map.entry_size = 0;
		header.image.x = 0;
		header.image.y = 0;
		header.image.width = static_cast<std::uint16_t>(info.width());
		header.image.height = static_cast<std::uint16_t>(info.height());
		header.image.pixel_depth = static_cast<std::uint8_t>(info.pixel_size() * 8);
		header.image.descriptor = (info.pixel_format() == PixelFormat::Bgra32) ? 8 : 0;

		switch (info.orientation())
		{
		case ImageOrientation::XRightYDown: header.image.descriptor |= tgaTopLeft; break;
		case ImageOrientation::XRightYUp: header.image.descriptor |= tgaBottomLeft; break;
		case ImageOrientation::XLeftYDown: header.image.descriptor |= tgaTopRight; break;
		case ImageOrientation::XLeftYUp: header.image.descriptor |= tgaBottomRight; break;
		}

		writer.reserve(sizeof header + info.frame_size());
		if (!writer.write(header))
			return false;

		auto scanline = static_cast<const std::uint8_t*>(data);
		for (size_t row = 0; row < info.height(); ++row)
		{
			if (!writer.write(scanline, info.row_size()))
				return false;
			scanline += info.row_size();
		}

		return true;
	}
}
