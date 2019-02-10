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

#define Y_USE_JPEG 1
#define Y_USE_PNG 1

#include <yttrium/storage/writer.h>
#include "../../libs/core/image/formats/bmp.h"
#include "../../libs/core/image/formats/dds.h"
#include "../../libs/core/image/formats/ico.h"
#include "../../libs/core/image/formats/tga.h"
#include "../../libs/core/image/formats.h"
#include "../core/image_formats.h"

#include <cstring>

using namespace Yttrium;

namespace
{
	bool write_color_gradient(Writer& writer, bool with_alpha, ImageOrientation orientation)
	{
		const auto image = ::make_test_image(with_alpha, orientation);
		return writer.write_all(image.data(), image.info().frame_size());
	}
}

int main()
{
	{
		const auto image = ::make_gray8_test_image<16>();

		TgaHeader header;
		std::memset(&header, 0, sizeof header);
		header.image_type = TgaImageType::BlackAndWhite;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 8;
		header.image.descriptor = tgaTopLeft;

		Writer writer{ "tests/core/data/gradient8.tga" };
		if (writer.write(header))
			writer.write_all(image.data(), image.info().frame_size());
	}
	{
		BmpFileHeader file_header;
		file_header.file_type = BmpFileType::Bm;
		file_header.file_size = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + 16 * 16 * 3;
		file_header.reserved = 0;
		file_header.data_offset = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);

		BmpInfoHeader info_header;
		info_header.header_size = sizeof info_header;
		info_header.width = 16;
		info_header.height = -16;
		info_header.planes = 1;
		info_header.bits_per_pixel = 24;
		info_header.compression = BmpCompression::Rgb;
		info_header.image_size = 0;
		info_header.x_pixels_per_meter = 4724; // 120 dpi.
		info_header.y_pixels_per_meter = 4724;
		info_header.used_colors = 0;
		info_header.required_colors = 0;

		Writer writer{ "tests/core/data/gradient24.bmp" };
		if (writer.write(file_header) && writer.write(info_header))
			::write_color_gradient(writer, false, ImageOrientation::XRightYDown);
	}
	{
		const auto image = ::make_test_image(false, ImageOrientation::XRightYDown);
		Writer writer{ "tests/core/data/gradient24_v2.png" };
		write_png_2(writer, image.info(), image.data());
	}
	{
		const auto image = ::make_test_image(false, ImageOrientation::XRightYDown);
		Writer writer{ "tests/core/data/gradient24_v3.png" };
		write_png_3(writer, image.info(), image.data());
	}
	{
		TgaHeader header;
		std::memset(&header, 0, sizeof header);
		header.image_type = TgaImageType::TrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 24;
		header.image.descriptor = tgaTopLeft;

		Writer writer{ "tests/core/data/gradient24.tga" };
		if (writer.write(header))
			::write_color_gradient(writer, false, ImageOrientation::XRightYDown);
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
			::write_color_gradient(writer, true, ImageOrientation::XRightYDown);
	}
	{
		constexpr std::uint32_t mask_data_size = (16 * 16 / 8) * 2; // Two 16x16 bitmasks (XOR bitmask and AND bitmask).
		constexpr std::uint32_t image_data_size = 16 * 16 * 4;

		IcoFileHeader file_header;
		file_header.reserved = 0;
		file_header.type = IcoFileType::Ico;
		file_header.count = 1;

		IcoImageHeader image_header;
		image_header.width = 16;
		image_header.height = 16;
		image_header.color_count = 0;
		image_header.reserved = 0;
		image_header.ico.color_planes = 1;
		image_header.ico.bits_per_pixel = 32;
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
		if (writer.write(file_header) && writer.write(image_header) && writer.write(bitmap_header) && ::write_color_gradient(writer, true, ImageOrientation::XRightYUp))
		{
			const auto mask_data_buffer = std::make_unique<std::uint8_t[]>(mask_data_size);
			std::memset(mask_data_buffer.get(), 0, mask_data_size);
			writer.write(mask_data_buffer.get(), mask_data_size);
		}
	}
	{
		TgaHeader header;
		std::memset(&header, 0, sizeof header);
		header.image_type = TgaImageType::TrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 32;
		header.image.descriptor = tgaTopLeft | 8;

		Writer writer{ "tests/core/data/gradient32.tga" };
		if (writer.write(header))
			::write_color_gradient(writer, true, ImageOrientation::XRightYDown);
	}
}
