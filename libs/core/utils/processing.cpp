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

#include "processing.h"

#include <algorithm>
#include <limits>

namespace Yttrium
{
	void add_saturate_i16(void* dst, const void* src, size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			const auto x = int32_t{ static_cast<int16_t*>(dst)[i] } + int32_t{ static_cast<const int16_t*>(src)[i] };
			static_cast<int16_t*>(dst)[i] = static_cast<int16_t>(std::clamp<decltype(x)>(x, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max()));
		}
	}

	void duplicate_i16(void* dst, const void* src, size_t src_count)
	{
		for (size_t i = 0; i < src_count; ++i)
		{
			const auto x = static_cast<const int16_t*>(src)[i];
			static_cast<int16_t*>(dst)[2 * i] = x;
			static_cast<int16_t*>(dst)[2 * i + 1] = x;
		}
	}
}
