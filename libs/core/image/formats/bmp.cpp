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

#include "bmp.h"

#include <yttrium/image.h>
#include <yttrium/storage/reader.h>
#include "../formats.h"

#include <limits>

namespace Yttrium
{
	std::optional<ImageFormat> read_bmp_header(Reader& reader)
	{
		BmpPrefix prefix;
		if (!reader.read(prefix)
			|| prefix.magic != BmpMagic::Bm)
			return {};

		BmpHeaders headers;
		if (!reader.read(headers)
			|| headers.info.header_size < sizeof headers.info
			|| headers.info.width < 0
			|| headers.info.planes != 1
			|| headers.info.compression != BmpCompression::Rgb)
			return {};

		PixelFormat pixel_format;
		switch (headers.info.bits_per_pixel)
		{
		case 24: pixel_format = PixelFormat::Bgr24; break;
		case 32: pixel_format = PixelFormat::Bgra32; break; // Non-standard, actually is BGRX with an unused byte.
		default: return {};
		}

		if (!reader.seek(headers.file.data_offset))
			return {};

		std::optional<ImageFormat> format;
		if (headers.info.height >= 0)
			format.emplace(headers.info.width, headers.info.height, pixel_format, ImageOrientation::XRightYUp);
		else
			format.emplace(headers.info.width, -headers.info.height, pixel_format, ImageOrientation::XRightYDown);
		return format;
	}
}
