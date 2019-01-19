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

#include <yttrium/storage/writer.h>
#include "../../libs/core/image/formats/bmp.h"
#include "../../libs/core/image/formats/dds.h"
#include "../../libs/core/image/formats/ico.h"
#include "../../libs/core/image/formats/tga.h"
#include "../core/image_formats.h"

#include <cstring>

using namespace Yttrium;

namespace
{
	template <bool with_alpha, bool flip_vertically = false>
	bool write_color_gradient(Writer& writer)
	{
		auto image = make_test_image(with_alpha);
		if (flip_vertically)
			image.flip_vertically();
		return writer.write_all(image.data(), image.format().frame_size());
	}
}

int main()
{
	{
		BmpPrefix prefix;
		prefix.magic = BmpMagic::Bm;

		BmpHeaders headers;
		headers.file.file_size = sizeof prefix + sizeof headers + 16 * 16 * 3;
		headers.file.reserved = 0;
		headers.file.data_offset = sizeof prefix + sizeof headers;
		headers.info.header_size = sizeof headers.info;
		headers.info.width = 16;
		headers.info.height = -16;
		headers.info.planes = 1;
		headers.info.bits_per_pixel = 24;
		headers.info.compression = BmpCompression::Rgb;
		headers.info.image_size = 0;
		headers.info.x_pixels_per_meter = 4724; // 120 dpi.
		headers.info.y_pixels_per_meter = 4724;
		headers.info.used_colors = 0;
		headers.info.required_colors = 0;

		Writer writer{ "tests/core/data/gradient24.bmp" };
		if (writer.write(prefix) && writer.write(headers))
			::write_color_gradient<false>(writer);
	}
	{
		TgaHeader header;
		std::memset(&header, 0, sizeof header);
		header.image_type = tgaTrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 24;
		header.image.descriptor = tgaTopLeft;

		Writer writer{ "tests/core/data/gradient24.tga" };
		if (writer.write(header))
			::write_color_gradient<false>(writer);
	}
	{
		DDS_HEADER header;
		std::memset(&header, 0, sizeof header);
		header.dwMagic = DDS_HEADER::MAGIC;
		header.dwSize = DDS_HEADER::SIZE;
		header.dwFlags = DDS_HEADER_FLAGS_TEXTURE;
		header.dwHeight = 16;
		header.dwWidth = 16;
		header.ddspf.dwSize = DDS_PIXELFORMAT::SIZE;
		header.ddspf.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
		header.ddspf.dwRGBBitCount = 32;
		header.ddspf.dwRBitMask = 0x00FF0000;
		header.ddspf.dwGBitMask = 0x0000FF00;
		header.ddspf.dwBBitMask = 0x000000FF;
		header.ddspf.dwABitMask = 0xFF000000;
		header.dwCaps = DDSCAPS_TEXTURE;

		Writer writer{ "tests/core/data/gradient32.dds" };
		if (writer.write(header))
			::write_color_gradient<true>(writer);
	}
	{
		constexpr std::uint32_t mask_data_size = (16 * 16 / 8) * 2; // Two 16x16 bitmasks (XOR bitmask and AND bitmask).
		constexpr std::uint32_t image_data_size = 16 * 16 * 4;

		IcoFileHeader file_header;
		file_header.reserved = 0;
		file_header.type = IcoFileHeader::Type_Ico;
		file_header.count = 1;

		IcoImageHeader image_header;
		image_header.width = 16;
		image_header.height = 16;
		image_header.color_count = 0;
		image_header.reserved = 0;
		image_header.color_planes = 1;
		image_header.bits_per_pixel = 32;
		image_header.data_size = sizeof(BmpInfoHeader) + image_data_size + mask_data_size;
		image_header.data_offset = sizeof file_header + sizeof image_header;

		BmpInfoHeader bitmap_header;
		bitmap_header.header_size = sizeof bitmap_header;
		bitmap_header.width = 16;
		bitmap_header.height = 16 * 2; // Combined height for two bitmasks.
		bitmap_header.planes = 1;
		bitmap_header.bits_per_pixel = 32;
		bitmap_header.compression = BmpCompression::Rgb;
		bitmap_header.image_size = image_data_size + mask_data_size;
		bitmap_header.x_pixels_per_meter = 0;
		bitmap_header.y_pixels_per_meter = 0;
		bitmap_header.used_colors = 0;
		bitmap_header.required_colors = 0;

		Writer writer{ "tests/core/data/gradient32.ico" };
		if (writer.write(file_header) && writer.write(image_header) && writer.write(bitmap_header) && ::write_color_gradient<true, true>(writer))
		{
			const auto mask_data_buffer = std::make_unique<std::uint8_t[]>(mask_data_size);
			std::memset(mask_data_buffer.get(), 0, mask_data_size);
			writer.write(mask_data_buffer.get(), mask_data_size);
		}
	}
	{
		TgaHeader header;
		std::memset(&header, 0, sizeof header);
		header.image_type = tgaTrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 32;
		header.image.descriptor = tgaTopLeft | 8;

		Writer writer{ "tests/core/data/gradient32.tga" };
		if (writer.write(header))
			::write_color_gradient<true>(writer);
	}
}
