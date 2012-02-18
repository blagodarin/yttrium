#include <Yttrium/string.hpp>

#include <cstdio>  // sprintf
#include <cstring> // memcpy, memmove

#include <Yttrium/assert.hpp>
#include <Yttrium/utils.hpp>

namespace Yttrium
{

enum
{
	StringReserve   =  16, ///< Minimal memory block size allocated by the String.
	StringGrowBound = 256, ///< The boundary until String size will double.
	StringGrowStep  = 256, ///< Linear String grow step after the boundary.
};

String::String(const String &string)
	: StaticString(string._size)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(string._allocator)
{
	init(string._text, _size);
}

String::String(const StaticString &string, Allocator *allocator)
	: StaticString(string._size)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init(string._text, _size);
}

String::String(const char *text, size_t size, Allocator *allocator)
	: StaticString(size)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init(text, size);
}

String::String(const char *text, Allocator *allocator)
	: StaticString(strlen(text))
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init(text, _size);
}

String::String(size_t size, Allocator *allocator)
	: _buffer_size(max<size_t>(size, StringReserve))
	, _allocator(allocator)
{
	init(&Null, 0);
}

String::String(const StaticString &left, const StaticString &right, Allocator *allocator)
	: StaticString(left._size + right._size)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	memcpy(_text, left._text, left._size);
	memcpy(&_text[left._size], right._text, right._size);
	_text[_size] = '\0';
}

String::String(const StaticString &left, char right, Allocator *allocator)
	: StaticString(left._size + 1)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	memcpy(_text, left._text, left._size);
	_text[left._size] = right;
	_text[_size] = '\0';
}

String::String(char left, const StaticString &right, Allocator *allocator)
	: StaticString(1 + right._size)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	_text[0] = left;
	memcpy(&_text[1], right._text, right._size);
	_text[_size] = '\0';
}

const String::Reference String::Ref = String::Reference();

String &String::append(const char *text, size_t size)
{
	size_t new_size = _size + size;
	if (_buffer_size)
	{
		grow(new_size + 1);
	}
	else
	{
		char *old_text = init(new_size + 1);
		memcpy(_text, old_text, _size);
	}
	memcpy(&_text[_size], text, size);
	_text[new_size] = '\0';
	_size = new_size;
	return *this;
}

String &String::append(char symbol, size_t count)
{
	size_t new_size = _size + count;
	if (_buffer_size)
	{
		grow(new_size + 1);
	}
	else
	{
		char *old_text = init(new_size + 1);
		memcpy(_text, old_text, _size);
	}
	memset(&_text[_size], symbol, count);
	_text[new_size] = '\0';
	_size = new_size;
	return *this;
}

String &String::append_dec(int64_t value, int width, bool zeros)
{
	char buffer[20];

	uint64_t uvalue;

	if (value >= 0)
	{
		uvalue = value;
	}
	else
	{
		uvalue = -value;
		--width;
	}

	int i = Y_LENGTH_OF(buffer);

	do
	{
		buffer[--i] = uvalue % 10 + '0';
		uvalue /= 10;
		--width;
	} while (uvalue);

	if (zeros)
	{
		if (value < 0)
		{
			append('-');
		}
		if (width > 0)
		{
			append('0', width);
		}
	}
	else
	{
		if (width > 0)
		{
			append(' ', width);
		}
		if (value < 0)
		{
			append('-');
		}
	}

	append(&buffer[i], Y_LENGTH_OF(buffer) - i);
	return *this;
}

String &String::append_dec(uint64_t value, int width, bool zeros)
{
	char buffer[20];

	int i = Y_LENGTH_OF(buffer);

	do
	{
		buffer[--i] = value % 10 + '0';
		value /= 10;
		--width;
	} while (value);

	if (width > 0)
	{
		append(zeros ? '0' : ' ', width);
	}

	append(&buffer[i], Y_LENGTH_OF(buffer) - i);
	return *this;
}

String &String::append_dec(float value) // NOTE: Terrible terrible implementation.
{
	char buffer[32];
	int  size;

	sprintf(buffer, "%f%n", static_cast<double>(value), &size);
	append(buffer, size);
	return *this;
}

String &String::append_dec(double value) // NOTE: Terrible terrible implementation.
{
	char buffer[32];
	int  size;

	sprintf(buffer, "%f%n", value, &size);
	append(buffer, size);
	return *this;
}

String &String::clear() noexcept
{
	if (_buffer_size)
	{
		size_t *references = reinterpret_cast<size_t *>(_text) - 1;
		if (*references == 1)
		{
			_text[0] = '\0';
			_size = 0;
			return *this;
		}
		--*references;
		_buffer_size = 0;
	}
	_text = const_cast<char *>(&Null);
	_size = 0;
	return *this;
}

String String::escaped(const char *symbols, char with) const
{
	String result(_size); // The best-case assumption.

	const char *end = _text + _size;

	for (const char *i = _text; i != end; ++i)
	{
		for (const char *j = symbols; *j; ++j)
		{
			if (*i == *j)
			{
				result.append(with);
				break;
			}
		}
		result.append(*i);
	}
	return result;
}

void String::insert(char symbol, size_t index)
{
	if (index > _size)
	{
		return;
	}

	size_t buffer_size = _size + 2;

	if (_buffer_size)
	{
		grow(buffer_size);
		if (index < _size)
		{
			memmove(_text + index + 1, _text + index, _size - index);
		}
	}
	else
	{
		char *old_text = init(buffer_size);
		if (index)
		{
			memcpy(_text, old_text, index);
		}
		if (index < _size)
		{
			memcpy(_text + index + 1, old_text + index, _size - index);
		}
	}
	_text[index] = symbol;
	++_size;
	_text[_size] = '\0';
}

void String::remove(size_t index)
{
	if (index >= _size)
	{
		return;
	}

	size_t *references = nullptr;

	if (_buffer_size)
	{
		references = reinterpret_cast<size_t *>(_text) - 1;
		if (*references == 1)
		{
			--_size;
			memmove(_text + index, _text + index + 1, _size - index);
			_text[_size] = '\0';
			return;
		}
	}

	char *old_text = init(_size);
	if (index)
	{
		memcpy(_text, old_text, index);
	}
	memcpy(_text + index, old_text + index + 1, _size - index);
	_text[--_size] = '\0';

	if (references)
	{
		--*references;
	}
}

void String::reserve(size_t size)
{
	size_t buffer_size = size + 1;
	if (_buffer_size)
	{
		grow(buffer_size);
	}
	else
	{
		char *old_text = init(max(_size + 1, buffer_size));
		memcpy(_text, old_text, _size + 1);
		_text[_size] = '\0';
	}
}


void String::truncate(size_t size)
{
	if (size >= _size)
	{
		return;
	}

	size_t *references = nullptr;

	if (_buffer_size)
	{
		references = reinterpret_cast<size_t *>(_text) - 1;
		if (*references == 1)
		{
			_text[size] = '\0';
			_size = size;
			return;
		}
	}

	char *old_text = init(size + 1);
	memcpy(_text, old_text, size);
	_text[size] = '\0';
	_size = size;

	if (references)
	{
		--*references;
	}
}

String &String::set(char symbol)
{
	if (_buffer_size)
	{
		grow(2);
	}
	else
	{
		init(2);
	}

	_text[0] = symbol;
	_text[1] = '\0';
	_size = 1;

	return *this;
}

String &String::set(const char *text, size_t size)
{
	size_t buffer_size = size + 1;

	if (_buffer_size)
	{
		grow(buffer_size);
	}
	else
	{
		init(buffer_size);
	}

	memcpy(_text, text, size);
	_text[size] = '\0';
	_size = size;

	return *this;
}

void String::grow(size_t buffer_size)
{
	Y_ASSERT(_buffer_size);

	size_t *references = reinterpret_cast<size_t *>(_text) - 1;
	if (*references != 1)
	{
		const char *old_text = _text;
		init(buffer_size);
		memcpy(_text, old_text, _size);
		_text[_size] = '\0';
		--*references;
	}
	else if (_buffer_size < buffer_size)
	{
		size_t adjusted_size = max(buffer_size,
			(_buffer_size < StringGrowBound
				? _buffer_size * 2
				: _buffer_size + StringGrowStep));

		size_t *pointer = static_cast<size_t *>(
			_allocator->reallocate(
				reinterpret_cast<size_t *>(_text) - 1,
				sizeof(size_t) + adjusted_size));

		_text = reinterpret_cast<char *>(pointer + 1);
		_buffer_size = adjusted_size;
	}
}

void String::init()
{
	Y_ASSERT(_buffer_size);

	if (_allocator) // NOTE: Rely on this at your own risk.
	{
		size_t *pointer = static_cast<size_t *>(_allocator->allocate(sizeof(size_t) + _buffer_size));
		*pointer = 1;
		_text = reinterpret_cast<char *>(pointer + 1);
	}
}

void String::init(const char *string, size_t size)
{
	init();
	memcpy(_text, string, size);
	_text[size] = '\0';
}

char *String::init(size_t buffer_size)
{
	Y_ASSERT(!_buffer_size);

	if (_allocator) // NOTE: Rely on this at your own risk.
	{
		size_t adjusted_size = max<size_t>(buffer_size, StringReserve);
		size_t *pointer = static_cast<size_t *>(_allocator->allocate(sizeof(size_t) + adjusted_size));
		*pointer = 1;
		char *old_text = _text;
		_text = reinterpret_cast<char *>(pointer + 1);
		_buffer_size = adjusted_size;
		return old_text;
	}
	return _text;
}

} // namespace Yttrium
