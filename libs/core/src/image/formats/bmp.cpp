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
