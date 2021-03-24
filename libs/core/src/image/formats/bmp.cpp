// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "bmp.h"

#include <yttrium/image/image.h>
#include <yttrium/storage/reader.h>
#include "../formats.h"

#include <limits>

namespace Yt
{
	bool read_bmp_header(Reader& reader, ImageInfo& info)
	{
		BmpFileHeader file_header;
		if (!reader.read(file_header)
			|| file_header.file_type != BmpFileType::Bm
			|| file_header.reserved != 0)
			return false;

		BmpInfoHeader info_header;
		if (!reader.read(info_header)
			|| info_header.header_size < sizeof info_header
			|| info_header.width <= 0
			|| info_header.height == 0
			|| info_header.planes != 1
			|| info_header.compression != BmpCompression::Rgb)
			return false;

		PixelFormat pixel_format;
		switch (info_header.bits_per_pixel)
		{
		case 24: pixel_format = PixelFormat::Bgr24; break;
		case 32: pixel_format = PixelFormat::Bgra32; break; // Non-standard, actually is BGRX with an unused byte.
		default: return false;
		}

		if (!reader.seek(file_header.data_offset))
			return false;

		if (info_header.height >= 0)
			info = { static_cast<std::size_t>(info_header.width), static_cast<std::size_t>(info_header.height), pixel_format, ImageOrientation::XRightYUp };
		else
			info = { static_cast<std::size_t>(info_header.width), static_cast<std::size_t>(-info_header.height), pixel_format, ImageOrientation::XRightYDown };
		return true;
	}
}
