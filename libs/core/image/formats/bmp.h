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

#pragma once

#include "../../utils/fourcc.h"

namespace Yttrium
{
#pragma pack(push, 1)

	struct BmpPrefix
	{
		uint16_t magic;
	};

	struct BmpInfoHeader
	{
		uint32_t header_size;
		int32_t width;
		int32_t height;
		uint16_t planes;
		uint16_t bits_per_pixel;
		uint32_t compression;
		uint32_t image_size;
		int32_t x_pixels_per_meter;
		int32_t y_pixels_per_meter;
		uint32_t used_colors;
		uint32_t required_colors;
	};

	struct BmpHeaders
	{
		struct
		{
			uint32_t file_size;
			uint32_t reserved;
			uint32_t data_offset;
		} file;
		BmpInfoHeader info;
	};

#pragma pack(pop)

	namespace BmpCompression
	{
		inline constexpr uint16_t Rgb = 0;
	}

	namespace BmpMagic
	{
		inline constexpr uint16_t Bm = "BM"_twocc;
	}
}
