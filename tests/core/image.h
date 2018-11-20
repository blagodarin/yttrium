//
// Copyright 2018 Sergei Blagodarin
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

#include <yttrium/image.h>
#include "../../libs/core/image/utils.h"

#include <cstring>

namespace Yttrium
{
	inline bool operator==(const Image& a, const Image& b)
	{
		const auto format = a.format();
		if (format != b.format())
			return false;
		for (size_t i = 0; i < format.height(); ++i)
		{
			const auto a_row = static_cast<const uint8_t*>(a.data()) + i * format.row_size();
			const auto b_row = static_cast<const uint8_t*>(b.data()) + i * format.row_size();
			if (std::memcmp(a_row, b_row, unaligned_image_row_size(format.width(), format.bits_per_pixel())))
				return false;
		}
		return true;
	}

	inline bool operator!=(const Image& a, const Image& b)
	{
		return !(a == b);
	}
}
