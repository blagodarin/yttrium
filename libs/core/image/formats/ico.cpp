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

#include "ico.h"

#include <yttrium/image.h>
#include <yttrium/storage/reader.h>
#include "../formats.h"
#include "bmp.h"

namespace Yttrium
{
	std::optional<ImageFormat> read_ico_header(Reader& reader)
	{
		IcoFileHeader file_header;
		if (!reader.read(file_header)
			|| file_header.reserved != 0
			|| file_header.type != IcoFileHeader::Type_Ico
			|| file_header.count != 1)
			return {};

		IcoImageHeader image_header;
		if (!reader.read(image_header)
			|| image_header.width == 0
			|| image_header.height == 0
			|| image_header.color_count != 0
			|| image_header.reserved != 0
			|| image_header.color_planes != 1)
			return {};

		BmpInfoHeader bitmap_header;
		if (!reader.seek(image_header.data_offset)
			|| !reader.read(bitmap_header)
			|| bitmap_header.header_size < sizeof bitmap_header
			|| bitmap_header.width != image_header.width
			|| bitmap_header.height != image_header.height * 2
			|| bitmap_header.planes != 1
			|| bitmap_header.bits_per_pixel != image_header.bits_per_pixel
			|| bitmap_header.compression != BmpCompression::Rgb)
			return {};

		PixelFormat pixel_format;
		switch (bitmap_header.bits_per_pixel)
		{
		case 24: pixel_format = PixelFormat::Bgr24; break;
		case 32: pixel_format = PixelFormat::Bgra32; break; // Non-standard, actually is BGRX with an unused byte.
		default: return {};
		}

		std::optional<ImageFormat> format;
		if (bitmap_header.height >= 0)
			format.emplace(bitmap_header.width, bitmap_header.height / 2, pixel_format, ImageOrientation::XRightYUp);
		else
			format.emplace(bitmap_header.width, -bitmap_header.height / 2, pixel_format, ImageOrientation::XRightYDown);
		return format;
	}
}
