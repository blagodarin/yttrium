#include <Yttrium/static_string.hpp>

namespace Yttrium
{

int StaticString::compare(const StaticString &string) const throw()
{
	if (_size < string._size)
	{
		int result = memcmp(_text, string._text, _size);
		return (result ? result : (string._text[_size] == '\0' ? 0 : -1));
	}
	else
	{
		int result = memcmp(_text, string._text, string._size);
		return (result ? result : (_text[string._size] == '\0' ? 0 : +1));
	}
}

size_t StaticString::find_first(char symbol, size_t offset) const throw()
{
	if (offset < _size)
	{
		const char *end = _text + _size;

		for (const char *c = &_text[offset]; c != end; ++c)
		{
			if (*c == symbol)
			{
				return c - _text;
			}
		}
	}
	return End;
}

size_t StaticString::find_last(char symbol, size_t offset) const throw()
{
	if (offset >= _size)
	{
		offset = _size - 1;
	}

	const char *end = _text - 1;

	for (const char *c = &_text[offset]; c != end; --c)
	{
		if (*c == symbol)
		{
			return c - _text;
		}
	}
	return End;
}

const size_t StaticString::End;

const char StaticString::Null;

} // namespace Yttrium
