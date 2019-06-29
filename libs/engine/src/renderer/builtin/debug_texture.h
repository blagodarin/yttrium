//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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
	namespace DebugTexture
	{
		constexpr uint8_t char_width = 8;
		constexpr uint8_t char_height = 16;

		constexpr uint8_t first_char = 33;
		constexpr uint8_t last_char = 126;

		constexpr uint8_t cursor_index = 0;
		constexpr uint8_t rect_index = 127;

		constexpr uint16_t width = 128;
		constexpr uint16_t height = 128;

		extern const int32_t data[];

		extern const float coords[][2][2];
	}
}
