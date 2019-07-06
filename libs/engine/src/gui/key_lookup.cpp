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

/* ANSI-C code produced by gperf version 3.0.4 */
/* Command-line: gperf -CElt -e^ -F,\ Key::Null -LANSI-C -m2 -Nlookup_key */
/* Computed positions: -k'1-2,$' */
/* maximum key range = 137, duplicates = 0 */

#include "key_lookup.h"

namespace
{
	size_t hash(std::string_view text)
	{
		static const size_t asso_values[]{
			// clang-format off
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140,  69,
			140, 140,   4,   2,  68,  64,  63,  60,  59,  14,
			 18,  30,  28,  19,  58,  54,  53,  51, 140, 140,
			140,  67, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140,  66,  65,  62, 140, 140,  61,  55,  38,  35,
			 34,  11,  16,  50,  48,  27,  57,  41,  45,   3,
			  5,  21,   8,  56,   4,  10,   2,  13,  52,   1,
			 49,  47,  43, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
			140, 140, 140, 140, 140, 140,
			// clang-format on
		};

		auto result = text.size();
		if (result != 1)
			result += asso_values[static_cast<unsigned char>(text[1])];
		result += asso_values[static_cast<unsigned char>(text[0])];
		return result + asso_values[static_cast<unsigned char>(text[text.size() - 1])];
	}
}

namespace Yttrium
{
	Key lookup_key(std::string_view name)
	{
		enum
		{
			TOTAL_KEYWORDS = 107,
			MIN_WORD_LENGTH = 1,
			MAX_WORD_LENGTH = 9,
			MIN_HASH_VALUE = 3,
			MAX_HASH_VALUE = 139,
		};

		static const unsigned char lengthtable[]{
			// clang-format off
			0,  0,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  6,  7,
			0,  6,  4,  1,  0,  5,  0,  1,  6,  1,  4,  5,  4,  1,
			0,  1,  8,  2,  0,  1,  5,  9,  4,  1,  5,  1,  4,  4,
			0,  1,  6,  3,  2,  3,  6,  6,  4,  3,  4,  3,  2,  1,
			2,  1,  6,  3,  6,  1,  4,  6,  4,  4,  0,  4,  0,  1,
			4,  1,  0,  4,  2,  4,  4,  1,  2,  5,  4,  4,  4,  1,
			4,  4,  4,  1,  0,  5,  0,  1,  3,  3,  0,  1,  0,  1,
			3,  1,  0,  1,  0,  1,  0,  1,  4,  1,  0,  1,  0,  1,
			0,  1,  0,  1,  0,  1,  0,  1,  2,  1,  0,  1,  2,  1,
			2,  1,  0,  1,  5,  1,  0,  1,  2,  1,  0,  1,  4,  1,
			// clang-format on
		};

		static const struct
		{
			const char* name;
			Key key;
		} wordlist[]{
			{ "", Key::Null },
			{ "", Key::Null },
			{ "", Key::Null },
			{ "w", Key::W },
			{ "", Key::Null },
			{ "t", Key::T },
			{ "", Key::Null },
			{ "m", Key::M },
			{ "", Key::Null },
			{ "r", Key::R },
			{ "", Key::Null },
			{ "n", Key::N },
			{ "mwleft", Key::WheelLeft },
			{ "mwright", Key::WheelRight },
			{ "", Key::Null },
			{ "mwdown", Key::WheelDown },
			{ "mwup", Key::WheelUp },
			{ "p", Key::P },
			{ "", Key::Null },
			{ "print", Key::Print },
			{ "", Key::Null },
			{ "s", Key::S },
			{ "rshift", Key::RShift },
			{ "e", Key::E },
			{ "num+", Key::Add },
			{ "enter", Key::Enter },
			{ "num*", Key::Multiply },
			{ "u", Key::U },
			{ "", Key::Null },
			{ "1", Key::_1 },
			{ "numenter", Key::NumEnter },
			{ "up", Key::Up },
			{ "", Key::Null },
			{ "f", Key::F },
			{ "space", Key::Space },
			{ "semicolon", Key::Semicolon },
			{ "num1", Key::Num1 },
			{ "2", Key::_2 },
			{ "right", Key::Right },
			{ "5", Key::_5 },
			{ "num2", Key::Num2 },
			{ "num5", Key::Num5 },
			{ "", Key::Null },
			{ "o", Key::O },
			{ "mouse1", Key::Mouse1 },
			{ "ins", Key::Insert },
			{ "f1", Key::F1 },
			{ "f11", Key::F11 },
			{ "mouse2", Key::Mouse2 },
			{ "mouse5", Key::Mouse5 },
			{ "num4", Key::Num4 },
			{ "f12", Key::F12 },
			{ "num3", Key::Num3 },
			{ "end", Key::End },
			{ "f2", Key::F2 },
			{ "i", Key::I },
			{ "f5", Key::F5 },
			{ "4", Key::_4 },
			{ "mouse4", Key::Mouse4 },
			{ "esc", Key::Escape },
			{ "mouse3", Key::Mouse3 },
			{ "3", Key::_3 },
			{ "left", Key::Left },
			{ "lshift", Key::LShift },
			{ "down", Key::Down },
			{ "ralt", Key::RAlt },
			{ "", Key::Null },
			{ "pgdn", Key::PageDown },
			{ "", Key::Null },
			{ "d", Key::D },
			{ "pgup", Key::PageUp },
			{ "c", Key::C },
			{ "", Key::Null },
			{ "num9", Key::Num9 },
			{ "f4", Key::F4 },
			{ "num8", Key::Num8 },
			{ "num7", Key::Num7 },
			{ "b", Key::B },
			{ "f3", Key::F3 },
			{ "pause", Key::Pause },
			{ "num6", Key::Num6 },
			{ "num0", Key::Num0 },
			{ "num/", Key::Divide },
			{ "k", Key::K },
			{ "home", Key::Home },
			{ "num.", Key::Decimal },
			{ "num-", Key::Subtract },
			{ "z", Key::Z },
			{ "", Key::Null },
			{ "rctrl", Key::RControl },
			{ "", Key::Null },
			{ "l", Key::L },
			{ "f10", Key::F10 },
			{ "del", Key::Delete },
			{ "", Key::Null },
			{ "y", Key::Y },
			{ "", Key::Null },
			{ "h", Key::H },
			{ "tab", Key::Tab },
			{ "x", Key::X },
			{ "", Key::Null },
			{ "g", Key::G },
			{ "", Key::Null },
			{ "9", Key::_9 },
			{ "", Key::Null },
			{ "v", Key::V },
			{ "lalt", Key::LAlt },
			{ "8", Key::_8 },
			{ "", Key::Null },
			{ "7", Key::_7 },
			{ "", Key::Null },
			{ "a", Key::A },
			{ "", Key::Null },
			{ "q", Key::Q },
			{ "", Key::Null },
			{ "j", Key::J },
			{ "", Key::Null },
			{ "6", Key::_6 },
			{ "", Key::Null },
			{ "0", Key::_0 },
			{ "f9", Key::F9 },
			{ "/", Key::Slash },
			{ "", Key::Null },
			{ "`", Key::Grave },
			{ "f8", Key::F8 },
			{ "]", Key::RBracket },
			{ "f7", Key::F7 },
			{ ".", Key::Period },
			{ "", Key::Null },
			{ "-", Key::Minus },
			{ "lctrl", Key::LControl },
			{ "\\", Key::Backslash },
			{ "", Key::Null },
			{ "[", Key::LBracket },
			{ "f6", Key::F6 },
			{ "=", Key::Equals },
			{ "", Key::Null },
			{ ",", Key::Comma },
			{ "back", Key::Backspace },
			{ "'", Key::Apostrophe }
		};

		if (MIN_WORD_LENGTH <= name.size() && name.size() <= MAX_WORD_LENGTH)
		{
			const auto key = hash(name);
			if (key <= MAX_HASH_VALUE && name == std::string_view{ wordlist[key].name, lengthtable[key] })
				return wordlist[key].key;
		}

		return Key::Null;
	}
}
