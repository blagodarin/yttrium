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

#include <yttrium/image.h>

#include <cstring>

namespace Yttrium
{
	inline bool operator==(const Image& a, const Image& b) noexcept
	{
		const auto format = a.format();
		if (format != b.format())
			return false;
		for (std::size_t y = 0; y < format.height(); ++y)
		{
			const auto a_row = static_cast<const std::uint8_t*>(a.data()) + y * format.row_size();
			const auto b_row = static_cast<const std::uint8_t*>(b.data()) + y * format.row_size();
			if (std::memcmp(a_row, b_row, format.width() * format.pixel_size()))
				return false;
		}
		return true;
	}

	inline bool operator!=(const Image& a, const Image& b) noexcept
	{
		return !(a == b);
	}
}
