#include <Yttrium/static_string.hpp>

namespace Yttrium
{

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
