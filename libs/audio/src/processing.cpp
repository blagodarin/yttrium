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

#include "processing.h"

namespace Yt
{
	void convert_normalize_f32_i16(void* dst, const void* src, size_t count) noexcept
	{
		for (size_t i = 0; i < count; ++i)
			static_cast<float*>(dst)[i] = static_cast<float>(static_cast<const int16_t*>(src)[i]) / 32768.f;
	}

	void convert_normalize_duplicate_f32_i16(void* dst, const void* src, size_t src_count) noexcept
	{
		for (size_t i = 0; i < src_count; ++i)
		{
			const auto x = static_cast<float>(static_cast<const int16_t*>(src)[i]) / 32768.f;
			static_cast<float*>(dst)[2 * i] = x;
			static_cast<float*>(dst)[2 * i + 1] = x;
		}
	}

	void duplicate_i16(void* dst, const void* src, size_t src_count) noexcept
	{
		for (size_t i = 0; i < src_count; ++i)
		{
			const auto x = static_cast<const int16_t*>(src)[i];
			static_cast<int16_t*>(dst)[2 * i] = x;
			static_cast<int16_t*>(dst)[2 * i + 1] = x;
		}
	}

	void duplicate_f32(void* dst, const void* src, size_t src_count) noexcept
	{
		for (size_t i = 0; i < src_count; ++i)
		{
			const auto x = static_cast<const float*>(src)[i];
			static_cast<float*>(dst)[2 * i] = x;
			static_cast<float*>(dst)[2 * i + 1] = x;
		}
	}
}
