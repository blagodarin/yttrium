// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "ico.h"

#include <yttrium/image/image.h>
#include <yttrium/storage/reader.h>
#include "../formats.h"
#include "bmp.h"

namespace Yt
{
	bool read_ico_header(Reader& reader, ImageInfo& info)
	{
		IcoFileHeader file_header;
		if (!reader.read(file_header)
			|| file_header.reserved != 0
			|| file_header.type != IcoFileType::Ico
			|| file_header.count != 1)
			return false;

		IcoImageHeader image_header;
		if (!reader.read(image_header)
			|| image_header.color_count != 0
			|| image_header.reserved != 0
			|| image_header.ico.color_planes != 1
			|| image_header.ico.bits_per_pixel != 32)
			return false;

		const uint16_t width = image_header.width ? image_header.width : 256;
		const uint16_t height = image_header.height ? image_header.height : 256;

		BmpInfoHeader bitmap_header;
		if (!reader.seek(image_header.data_offset)
			|| !reader.read(bitmap_header)
			|| bitmap_header.header_size < sizeof bitmap_header
			|| bitmap_header.width != width
			|| bitmap_header.height != height * 2
			|| bitmap_header.planes != 1
			|| bitmap_header.bits_per_pixel != image_header.ico.bits_per_pixel
			|| bitmap_header.compression != BmpCompression::Rgb)
			return false;

		info = { width, height, PixelFormat::Bgra32, ImageOrientation::XRightYUp };
		return true;
	}
}
