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

#pragma once

#include <yttrium/utils/numeric.h>

namespace Yttrium
{
	enum class BmpFileType : uint16_t
	{
		Bm = make_cc('B', 'M'),
	};

	enum class BmpCompression : uint32_t
	{
		Rgb = 0,
	};

#pragma pack(push, 1)

	struct BmpFileHeader
	{
		BmpFileType file_type;
		uint32_t file_size;
		uint32_t reserved;
		uint32_t data_offset;
	};

	struct BmpInfoHeader
	{
		uint32_t header_size;
		int32_t width;
		int32_t height;
		uint16_t planes;
		uint16_t bits_per_pixel;
		BmpCompression compression;
		uint32_t image_size;
		int32_t x_pixels_per_meter;
		int32_t y_pixels_per_meter;
		uint32_t used_colors;
		uint32_t required_colors;
	};

#pragma pack(pop)
}
