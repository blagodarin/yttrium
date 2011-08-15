#include <cmath>   // pow
#include <cstdio>  // sprintf
#include <cstring> // memcpy, memmove

#include <Yttrium/assert.hpp>
#include <Yttrium/string.hpp>
#include <Yttrium/utils.hpp>

namespace Yttrium
{

enum
{
	StringReserve   =  16, ///< Minimal memory block size allocated by the String.
	StringGrowBound = 256, ///< The boundary until String size will double.
	StringGrowStep  = 256, ///< Linear String grow step after the boundary.
};

String::String(const char *string, Allocator *allocator)
	: _size(strlen(string))
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init(string, _size);
}

String::String(const char *string, size_t size, Allocator *allocator)
	: _size(size)
	, _buffer_size(max<size_t>(size + 1, StringReserve))
	, _allocator(allocator)
{
	init(string, size);
}

String::String(const StaticString &string, Allocator *allocator)
	: _size(string.size())
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init(string.text(), _size);
}

String::String(const String &string)
	: _size(string._size)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(string._allocator)
{
	init(string._text, _size);
}

String::String(const String &string, Allocator *allocator)
	: _size(string._size)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init(string._text, _size);
}

String::String(size_t size, Allocator *allocator)
	: _size(0)
	, _buffer_size(max<size_t>(size, StringReserve))
	, _allocator(allocator)
{
	init("", 0);
}

String::String(char left, const StaticString& right, Allocator *allocator)
	: _size(1 + right.size())
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	_text[0] = left;
	memcpy(&_text[1], right.text(), right.size());
	_text[_size] = '\0';
}

String::String(const char *left, const StaticString &right, Allocator *allocator)
	: _size(strlen(left) + right.size())
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	size_t left_size = _size - right.size();
	memcpy(_text, left, left_size);
	memcpy(&_text[left_size], right.text(), right.size());
	_text[_size] = '\0';
}

String::String(const StaticString &left, char right, Allocator *allocator)
	: _size(left.size() + 1)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	memcpy(_text, left.text(), left.size());
	_text[left.size()] = right;
	_text[_size] = '\0';
}

String::String(const StaticString &left, const char *right, Allocator *allocator)
	: _size(left.size() + strlen(right))
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	memcpy(_text, left.text(), left.size());
	memcpy(&_text[left.size()], right, _size - left.size());
	_text[_size] = '\0';
}

String::String(const StaticString &left, const StaticString &right, Allocator *allocator)
	: _size(left.size() + right.size())
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	memcpy(_text, left.text(), left.size());
	memcpy(&_text[left.size()], right.text(), right.size());
	_text[_size] = '\0';
}

String &String::append(const char *string, size_t size)
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
	memcpy(&_text[_size], string, size);
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
		char* old_text = init(new_size + 1);
		memcpy(_text, old_text, _size);
	}
	memset(&_text[_size], symbol, count);
	_text[new_size] = '\0';
	_size = new_size;
	return *this;
}

String &String::append_dec(int64_t value, int width, bool zeros)
{
	char buffer[21];

	buffer[Y_LENGTH_OF(buffer) - 1] = '\0';

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

	int i = Y_LENGTH_OF(buffer) - 1;

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

	append(&buffer[i], 20 - i);
	return *this;
}

String &String::append_dec(uint64_t value, int width, bool zeros)
{
	char buffer[21];

	buffer[Y_LENGTH_OF(buffer) - 1] = '\0';

	int i = Y_LENGTH_OF(buffer) - 1;

	do
	{
		buffer[--i] = value % 10 + '0';
		value /= 10;
		--width;
	}
	while (value);

	if (width > 0)
	{
		append(zeros ? '0' : ' ', width);
	}

	append(&buffer[i], 20 - i);
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

String &String::clear() throw()
{
	if (_buffer_size)
	{
		size_t *references = reinterpret_cast<size_t *>(_text) - 1;
		if (*references == 1)
		{
			_text[0] = '\0';
			return *this;
		}
		--*references;
		_buffer_size = 0;
	}
	_text = const_cast<char *>(&null);
	_size = 0;
	return *this;
}

size_t String::count(const char *symbols) const throw()
{
	size_t result = 0;
	for (const char *i = _text; *i; ++i)
	{
		for (const char *j = symbols; *j; ++j)
		{
			if (*i == *j)
			{
				++result;
				break;
			}
		}
	}
	return result;
}

String String::escaped(const char *symbols, char with) const
{
	String result(_size); // The best-case assumption.

	for (const char *i = _text; *i; ++i)
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
		char* old_text = init(buffer_size);
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

	size_t *references = NULL;

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

double String::to_double() const throw()
{
	double result = 0;

	const char* p = _text;
	bool negate_result = false;
	switch (*p)
	{
	case '-': negate_result = true; // Fallthrough.
	case '+': ++p;
	}

	// Whole.

	for (; *p >= '0' && *p <= '9'; ++p)
	{
		result = result * 10 + *p - '0';
	}

	// Fraction.

	if (*p == '.')
	{
		++p;
		double factor = 1;
		for (; *p >= '0' && *p <= '9'; ++p)
		{
			result = result * 10 + *p - '0';
			factor *= 10;
		}
		result /= factor;
	}

	// Exponent.

	if (*p == 'E' || *p == 'e')
	{
		double exp = 0;
		bool neg_exp = false;

		++p;
		switch (*p)
		{
		case '-': neg_exp = true; // Fallthrough.
		case '+': ++p;
		}

		// Exponent value.

		for (; *p >= '0' && *p <= '9'; ++p)
		{
			exp = exp * 10 + *p - '0';
		}
		if (neg_exp)
		{
			exp = -exp;
		}
		result *= pow(10.0, exp);
	}

	if (negate_result)
	{
		result = -result;
	}
	return result;
}

int32_t String::to_int32() const throw()
{
	int32_t result = 0;

	const char* p = _text;
	bool negate_result = false;
	switch (*p)
	{
	case '-': negate_result = true; // Fallthrough.
	case '+': ++p;
	}
	for (; *p >= '0' && *p <= '9'; ++p)
	{
		result = result * 10 + *p - '0';
	}
	if (negate_result)
	{
		result = -result;
	}
	return result;
}

int64_t String::to_int64() const throw()
{
	int64_t result = 0;

	const char* p = _text;
	bool negate_result = false;
	switch (*p)
	{
	case '-': negate_result = true; // Fallthrough.
	case '+': ++p;
	}
	for (; *p >= '0' && *p <= '9'; ++p)
	{
		result = result * 10 + *p - '0';
	}
	if (negate_result)
	{
		result = -result;
	}
	return result;
}

bool String::to_number(int32_t &value) const throw()
{
	bool negate_result = false;

	const char* p = _text;
	switch (*p)
	{
	case '-': negate_result = true; // Fallthrough.
	case '+': ++p;
	}

	char c = *p;
	if (!(c >= '0' && c <= '9'))
	{
		return false;
	}

	uint32_t result = 0;

	do
	{
		c -= '0';
		if (result < 0x0CCCCCCC)
		{
			result = result * 10 + c;
		}
		else
		{
			if (negate_result)
			{
				if (result <= (0x80000000u - c) / 10)
				{
					result = result * 10 + c;
				}
				else
				{
					return false;
				}
			}
			else
			{
				if (result <= (0x7FFFFFFFu - c) / 10)
				{
					result = result * 10 + c;
				}
				else
				{
					return false;
				}
			}
		}
		c = *++p;
	} while (c >= '0' && c <= '9');
	if (c == 0)
	{
		value = negate_result ? -result : result;
		return true;
	}
	return false;
}

bool String::to_number(double &value) const throw()
{
	bool negate_result = false;

	const char* p = _text;
	switch (*p)
	{
	case '-': negate_result = true; // Fallthrough.
	case '+': ++p;
	}

	// Whole.

	char c = *p;
	if (!(c >= '0' && c <= '9'))
	{
		return false;
	}

	double result = 0;

	do
	{
		result = result * 10 + c - '0';
		c = *++p;
	} while (c >= '0' && c <= '9');

	// Fraction.

	if (c == '.')
	{
		c = *++p;
		if (!(c >= '0' && c <= '9'))
		{
			return false;
		}
		double factor = 1;
		do
		{
			result = result * 10 + c - '0';
			factor *= 10;
			c = *++p;
		} while (c >= '0' && c <= '9');
		result /= factor;
	}

	// Exponent.

	if (c == 'E' || c == 'e')
	{
		bool neg_exp = false;
		switch (*++p)
		{
		case '-': neg_exp = true; // Fallthrough.
		case '+': ++p;
		}

		// Exponent value.

		c = *p;
		if (!(c >= '0' && c <= '9'))
		{
			return false;
		}
		double exp = 0;
		do
		{
			exp = exp * 10 + c - '0';
			c = *++p;
		} while (c >= '0' && c <= '9');
		if (neg_exp)
		{
			exp = -exp;
		}
		result *= pow(10.0, exp);
	}

	if (c == 0)
	{
		value = negate_result ? -result : result;
		return true;
	}
	return false;
}

double String::to_time() const throw()
{
	double result = 0;

	const char* p = _text;
	bool negate_result = false;
	switch (*p)
	{
	case '-': negate_result = true; // Fallthrough.
	case '+': ++p;
	}
	for (; *p >= '0' && *p <= '9'; ++p)
	{
		result = result * 10 + *p - '0';
	}
	if (*p == ':')
	{
		++p;
		double mins_or_secs = 0;
		for (; *p >= '0' && *p <= '9'; ++p)
		{
			mins_or_secs = mins_or_secs * 10 + *p - '0';
		}
		result = result * 60 + mins_or_secs;
	}
	if (*p == ':')
	{
		++p;
		double secs = 0;
		for (; *p >= '0' && *p <= '9'; ++p)
		{
			secs = secs * 10 + *p - '0';
		}
		result = result * 60 + secs;
	}
	if (*p == '.')
	{
		++p;
		double factor = 1;
		for (; *p >= '0' && *p <= '9'; ++p)
		{
			result = result * 10 + *p - '0';
			factor *= 10;
		}
		result /= factor;
	}
	if (negate_result)
	{
		result = -result;
	}
	return result;
}

uint32_t String::to_uint32() const throw()
{
	uint32_t result = 0;

	const char* p = _text;
	if (*p == '+')
	{
		++p;
	}
	for (; *p >= '0' && *p <= '9'; ++p)
	{
		result = result * 10 + *p - '0';
	}
	return result;
}

uint64_t String::to_uint64() const throw()
{
	uint64_t result = 0;

	const char* p = _text;
	if (*p == '+')
	{
		++p;
	}
	for (; *p >= '0' && *p <= '9'; ++p)
	{
		result = result * 10 + *p - '0';
	}
	return result;
}

void String::truncate(size_t size)
{
	if (size > _size)
	{
		return;
	}

	size_t *references = NULL;
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

String& String::set(char symbol)
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

String& String::set(const char *string, size_t size)
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
	memcpy(_text, string, size);
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

	size_t *pointer = static_cast<size_t *>(_allocator->allocate(sizeof(size_t) + _buffer_size));
	*pointer = 1;
	_text = reinterpret_cast<char *>(pointer + 1);
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

	size_t adjusted_size = max<size_t>(buffer_size, StringReserve);
	size_t *pointer = static_cast<size_t *>(_allocator->allocate(sizeof(size_t) + adjusted_size));
	*pointer = 1;
	char *old_text = _text;
	_text = reinterpret_cast<char *>(pointer + 1);
	_buffer_size = adjusted_size;
	return old_text;
}

const char String::null;

} // namespace Yttrium
