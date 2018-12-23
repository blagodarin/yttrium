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

#include <string_view>

namespace Yttrium
{
	namespace Utf8
	{
		inline constexpr bool is_continuation(char c) noexcept
		{
			return (static_cast<unsigned char>(c) & 0b1100'0000u) == 0b1000'0000u;
		}

		inline char32_t next_codepoint(std::string_view text, std::size_t& offset) noexcept
		{
			const auto part1 = static_cast<std::uint8_t>(text[offset++]);
			if (!(part1 & 0b1000'0000))
				return part1;

			if (offset == text.size())
				return 0;

			const auto part2 = char32_t{ static_cast<std::uint8_t>(text[offset++]) & 0b0011'1111u };
			if (!(part1 & 0b0010'0000))
				return ((part1 & 0b0001'1111u) << 6) + part2;

			if (offset == text.size())
				return 0;

			const auto part3 = char32_t{ static_cast<std::uint8_t>(text[offset++]) & 0b0011'1111u };
			if (!(part1 & 0b0001'0000))
				return ((part1 & 0b0000'1111u) << 12) + (part2 << 6) + part3;

			if (offset == text.size())
				return 0;

			const auto part4 = char32_t{ static_cast<std::uint8_t>(text[offset++]) & 0b0011'1111u };
			return ((part1 & 0b0000'0111u) << 18) + (part2 << 12) + (part3 << 6) + part4;
		}
	}
}
