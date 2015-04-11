#include <yttrium/key.h>

namespace Yttrium
{
	char KeyEvent::to_char() const
	{
		static const char char_count = 0x30;

		static const char lo_map[char_count] =
		{
			'0', '1', '2', '3', '4', '5', '6', '7',
			'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
			'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
			'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
			'w', 'x', 'y', 'z', ',', '.', ';','\'',
			'/','\\', '[', ']', '-', '=', '`', ' ',
		};

		static const char hi_map[char_count] =
		{
			')', '!', '@', '#', '$', '%', '^', '&',
			'*', '(', 'A', 'B', 'C', 'D', 'E', 'F',
			'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
			'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
			'W', 'X', 'Y', 'Z', '<', '>', ':', '"',
			'?', '|', '{', '}', '_', '+', '~', ' ',
		};

		if (key < Key::_0 || key > Key::Space)
			return '\0';

		const char* map = (modifiers & Shift) ? hi_map : lo_map;
		return map[KeyType(key) - KeyType(Key::_0)];
	}
}
