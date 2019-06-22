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

#include <yttrium/api.h>

#include <cstddef>
#include <cstdint>

namespace Yttrium
{
	Y_CORE_API void add_saturate_i16(void* dst, const void* src, size_t count) noexcept;
	Y_CORE_API void add_saturate_f32(void* dst, const void* src, size_t count) noexcept;

	void convert_normalize_f32_i16(void* dst, const void* src, size_t count) noexcept;

	void convert_normalize_duplicate_f32_i16(void* dst, const void* src, size_t src_count) noexcept;

	void duplicate_i16(void* dst, const void* src, size_t src_count) noexcept;
	void duplicate_f32(void* dst, const void* src, size_t src_count) noexcept;
}
