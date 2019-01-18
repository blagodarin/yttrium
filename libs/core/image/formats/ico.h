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

#include <cstdint>

namespace Yttrium
{
#pragma pack(push, 1)

	struct IcoFileHeader
	{
		static constexpr uint16_t Type_Ico = 1;

		uint16_t reserved;
		uint16_t type;
		uint16_t count;
	};

	struct IcoImageHeader
	{
		uint8_t width;
		uint8_t height;
		uint8_t color_count;
		uint8_t reserved;
		uint16_t color_planes;
		uint16_t bits_per_pixel;
		uint32_t data_size;
		uint32_t data_offset;
	};

#pragma pack(pop)
}
