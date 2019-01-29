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
		const auto info = a.info();
		if (info != b.info())
			return false;
		for (std::size_t y = 0; y < info.height(); ++y)
		{
			const auto a_row = static_cast<const std::uint8_t*>(a.data()) + y * info.stride();
			const auto b_row = static_cast<const std::uint8_t*>(b.data()) + y * info.stride();
			if (std::memcmp(a_row, b_row, info.width() * ImageInfo::pixel_size(info.pixel_format())))
				return false;
		}
		return true;
	}

	inline bool operator!=(const Image& a, const Image& b) noexcept
	{
		return !(a == b);
	}
}
