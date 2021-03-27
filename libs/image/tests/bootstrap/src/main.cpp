// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/logger.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include "../../../src/formats/bmp.h"
#include "../../../src/formats/dds.h"
#include "../../../src/formats/ico.h"
#include "../../src/formats.h"

#include <cstring>

namespace
{
	bool writeTestImageData(Yt::Writer& writer, bool withAlpha, Yt::ImageOrientation orientation)
	{
		const auto image = ::makeTestImage(withAlpha, orientation);
		return writer.write_all(image.data(), image.info().frame_size());
	}
}

int main()
{
	Yt::Logger logger;
#if YTTRIUM_IMAGE_BMP
	{
		Yt::BmpFileHeader file_header;
		file_header.file_type = Yt::BmpFileType::Bm;
		file_header.file_size = sizeof(Yt::BmpFileHeader) + sizeof(Yt::BmpInfoHeader) + 16 * 16 * 3;
		file_header.reserved = 0;
		file_header.data_offset = sizeof(Yt::BmpFileHeader) + sizeof(Yt::BmpInfoHeader);

		Yt::BmpInfoHeader info_header;
		info_header.header_size = sizeof info_header;
		info_header.width = 16;
		info_header.height = -16;
		info_header.planes = 1;
		info_header.bits_per_pixel = 24;
		info_header.compression = Yt::BmpCompression::Rgb;
		info_header.image_size = 0;
		info_header.x_pixels_per_meter = 4724; // 120 dpi.
		info_header.y_pixels_per_meter = 4724;
		info_header.used_colors = 0;
		info_header.required_colors = 0;

		Yt::Writer writer{ "gradient24.bmp" };
		if (writer.write(file_header) && writer.write(info_header))
			::writeTestImageData(writer, false, Yt::ImageOrientation::XRightYDown);
	}
#endif
#if YTTRIUM_IMAGE_JPEG
	{
		Yt::Buffer buffer;
		::makeTestImage(false, Yt::ImageOrientation::XRightYDown).save(Yt::Writer{ buffer }, Yt::ImageFormat::Jpeg);
		Yt::Writer{ "gradient24.jpg" }.write_all(buffer);
#	if YTTRIUM_IMAGE_TGA
		Yt::Image::load(*Yt::Source::from(std::move(buffer)))->save(Yt::Writer{ "gradient24.jpg.tga" }, Yt::ImageFormat::Tga);
#	endif
	}
#endif
#if YTTRIUM_IMAGE_DDS
	{
		Yt::DDS_HEADER header;
		std::memset(&header, 0, sizeof header);
		header.dwMagic = Yt::DDS_HEADER::MAGIC;
		header.dwSize = Yt::DDS_HEADER::SIZE;
		header.dwFlags = Yt::DDS_HEADER_FLAGS_TEXTURE;
		header.dwHeight = 16;
		header.dwWidth = 16;
		header.ddspf.dwSize = Yt::DDS_PIXELFORMAT::SIZE;
		header.ddspf.dwFlags = Yt::DDPF_ALPHAPIXELS | Yt::DDPF_RGB;
		header.ddspf.dwRGBBitCount = 32;
		header.ddspf.dwRBitMask = 0x00FF0000;
		header.ddspf.dwGBitMask = 0x0000FF00;
		header.ddspf.dwBBitMask = 0x000000FF;
		header.ddspf.dwABitMask = 0xFF000000;
		header.dwCaps = Yt::DDSCAPS_TEXTURE;

		Yt::Writer writer{ "gradient32.dds" };
		if (writer.write(header))
			::writeTestImageData(writer, true, Yt::ImageOrientation::XRightYDown);
	}
#endif
#if YTTRIUM_IMAGE_ICO
	{
		constexpr uint32_t mask_data_size = (16 * 16 / 8) * 2; // Two 16x16 bitmasks (XOR bitmask and AND bitmask).
		constexpr uint32_t image_data_size = 16 * 16 * 4;

		Yt::IcoFileHeader file_header;
		file_header.reserved = 0;
		file_header.type = Yt::IcoFileType::Ico;
		file_header.count = 1;

		Yt::IcoImageHeader image_header;
		image_header.width = 16;
		image_header.height = 16;
		image_header.color_count = 0;
		image_header.reserved = 0;
		image_header.ico.color_planes = 1;
		image_header.ico.bits_per_pixel = 32;
		image_header.data_size = sizeof(Yt::BmpInfoHeader) + image_data_size + mask_data_size;
		image_header.data_offset = sizeof file_header + sizeof image_header;

		Yt::BmpInfoHeader bitmap_header;
		bitmap_header.header_size = sizeof bitmap_header;
		bitmap_header.width = 16;
		bitmap_header.height = 16 * 2; // Combined height for two bitmasks.
		bitmap_header.planes = 1;
		bitmap_header.bits_per_pixel = 32;
		bitmap_header.compression = Yt::BmpCompression::Rgb;
		bitmap_header.image_size = image_data_size + mask_data_size;
		bitmap_header.x_pixels_per_meter = 0;
		bitmap_header.y_pixels_per_meter = 0;
		bitmap_header.used_colors = 0;
		bitmap_header.required_colors = 0;

		Yt::Writer writer{ "gradient32.ico" };
		if (writer.write(file_header) && writer.write(image_header) && writer.write(bitmap_header) && ::writeTestImageData(writer, true, Yt::ImageOrientation::XRightYUp))
		{
			const auto mask_data_buffer = std::make_unique<uint8_t[]>(mask_data_size);
			std::memset(mask_data_buffer.get(), 0, mask_data_size);
			writer.write(mask_data_buffer.get(), mask_data_size);
		}
	}
#endif
#if YTTRIUM_IMAGE_PNG
	::makeTestImage(false, Yt::ImageOrientation::XRightYDown).save(Yt::Writer{ "gradient24.png" }, Yt::ImageFormat::Png);
#endif
#if YTTRIUM_IMAGE_TGA
	::makeTestImageGray8<16>().save(Yt::Writer{ "gradient8.tga" }, Yt::ImageFormat::Tga);
	::makeTestImage(false, Yt::ImageOrientation::XRightYDown).save(Yt::Writer{ "gradient24.tga" }, Yt::ImageFormat::Tga);
	::makeTestImage(true, Yt::ImageOrientation::XRightYDown).save(Yt::Writer{ "gradient32.tga" }, Yt::ImageFormat::Tga);
#endif
}
