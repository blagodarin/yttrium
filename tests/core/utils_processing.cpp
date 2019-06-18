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

#include <yttrium/memory/buffer.h>
#include "../../libs/core/utils/processing.h"

#include <array>
#include <cstring>

#include <catch2/catch.hpp>

TEST_CASE("utils.add_saturate_i16")
{
	constexpr size_t n = 7;
	// clang-format off
	const std::array<int16_t, n> dst{ -32768, 32767,     -1, 32767,      1, 32767, 0 };
	const std::array<int16_t, n> src{ -32768, 32767, -32768,     1, -32768,    -1, 0 };
	const std::array<int16_t, n> res{ -32768, 32767, -32768, 32767, -32767, 32766, 0 };
	// clang-format on
	Yttrium::Buffer dst_buffer{ dst.size() * sizeof(int16_t), dst.data() };
	Yttrium::Buffer src_buffer{ src.size() * sizeof(int16_t), src.data() };
	Yttrium::add_saturate_i16(dst_buffer.data(), src_buffer.data(), src.size());
	CHECK(std::memcmp(dst_buffer.data(), res.data(), dst_buffer.size()) == 0);
}
