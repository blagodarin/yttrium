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

#include <yttrium/key.h>

namespace Yttrium
{
	char KeyEvent::to_char() const
	{
		static const char char_count = 0x30;

		static const char lo_map[char_count]{
			// clang-format off
			'0', '1', '2', '3', '4', '5', '6', '7',
			'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
			'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
			'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
			'w', 'x', 'y', 'z', ',', '.', ';','\'',
			'/','\\', '[', ']', '-', '=', '`', ' ',
			// clang-format on
		};

		static const char hi_map[char_count]{
			// clang-format off
			')', '!', '@', '#', '$', '%', '^', '&',
			'*', '(', 'A', 'B', 'C', 'D', 'E', 'F',
			'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
			'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
			'W', 'X', 'Y', 'Z', '<', '>', ':', '"',
			'?', '|', '{', '}', '_', '+', '~', ' ',
			// clang-format on
		};

		if (key < Key::_0 || key > Key::Space)
			return '\0';

		const char* map = (modifiers & Modifier::Shift) ? hi_map : lo_map;
		return map[to_underlying(key) - to_underlying(Key::_0)];
	}
}
