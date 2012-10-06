#include <Yttrium/string.h>

#include <Yttrium/allocator.h>
#include <Yttrium/assert.h>
#include <Yttrium/utils.h>

#include "ieee_float.h"

#include <cstdio>  // sprintf
#include <cstring> // memcpy, memmove, strlen

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
	: StaticString(string.size())
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init(string.text(), _size);
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
	: StaticString(left.size() + right.size())
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	memcpy(_text, left.text(), left.size());
	memcpy(&_text[left.size()], right.text(), right.size());
	_text[_size] = '\0';
}

String::String(const StaticString &left, char right, Allocator *allocator)
	: StaticString(left.size() + 1)
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	memcpy(_text, left.text(), left.size());
	_text[left.size()] = right;
	_text[_size] = '\0';
}

String::String(char left, const StaticString &right, Allocator *allocator)
	: StaticString(1 + right.size())
	, _buffer_size(max<size_t>(_size + 1, StringReserve))
	, _allocator(allocator)
{
	init();
	_text[0] = left;
	memcpy(&_text[1], right.text(), right.size());
	_text[_size] = '\0';
}

String::~String()
{
	if (_buffer_size)
	{
		size_t *references = reinterpret_cast<size_t *>(_text) - 1;
		if (!--*references)
		{
			_allocator->deallocate(references);
		}
	}
}

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

String &String::append(const char *text)
{
	return append(text, strlen(text));
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

namespace
{

StaticString negative_infinity = S("-INF");
StaticString positive_infinity = S("INF");
StaticString negative_nan = S("-NAN");
StaticString positive_nan = S("NAN");
StaticString negative_zero = S("-0");
StaticString positive_zero = S("0");

template <typename T>
StaticString float_to_string(char *buffer, T value, int max_fraction_digits = -1)
{
	// NOTE: This should provide exact float-to-string-to-float conversions.
	// NOTE: This won't work for non-IEEE floats and may not work on big endian architectures.

	typedef IeeeFloat<T> Float;

	typedef typename Float::FastSigned   FastSigned;
	typedef typename Float::FastUnsigned FastUnsigned;
	typedef typename Float::Union        Union;

	Union raw_value;

	raw_value.f = value;

	bool is_negative = raw_value.i & Float::SignMask;

	FastSigned exponent = (raw_value.i & Float::ExponentMask) >> Float::MantissaBits;
	FastUnsigned mantissa = raw_value.i & Float::MantissaMask;

	if (Y_UNLIKELY(!exponent))
	{
		if (Y_LIKELY(!mantissa))
		{
			return is_negative ? negative_zero : positive_zero;
		}

		// Base 2 logarithm calculation trick based on a Sean Eron Anderson snippet:
		// http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogIEEE64Float

		Union u;

		u.i = mantissa | ((Float::ExponentBias + Float::MantissaBits) << Float::MantissaBits);
		u.f -= Float::ImplicitOne;

		FastSigned shift = (Float::ExponentBias + Float::MantissaBits) - (u.i >> Float::MantissaBits);

		mantissa <<= shift;
		exponent = Float::DenormalizedExponent - shift;
	}
	else if (Y_UNLIKELY(exponent == Float::BiasedInfinityExponent))
	{
		if (Y_LIKELY(!mantissa))
		{
			return is_negative ? negative_infinity : positive_infinity;
		}
		else
		{
			return is_negative ? negative_nan : positive_nan;
		}
	}
	else
	{
		exponent -= Float::ExponentBias;
		mantissa |= Float::ImplicitOne;
	}

	// At this point, the absolute value is (mantissa / 0x800000) * pow(2, exponent),
	// with [0x800000, 0xFFFFFF] mantissa range, and [-147, 127] exponent range.

	if (Y_UNLIKELY(exponent > Float::MantissaBits)) // The precision exceeds 1.
	{
		// TODO: Implement.
	}
	else if (exponent < -1) // The absolute value is in (0; 0.5) range.
	{
		// TODO: Implement.
	}
	else
	{
		FastSigned whole_bits = exponent + 1;
		FastSigned fraction_bits = Float::MantissaBits - exponent;

		FastSigned whole_value = mantissa >> fraction_bits;

		char *begin = buffer + (10 + 3 * whole_bits) / 10 + 1;
		char *end = begin;

		do
		{
			*--begin = '0' + whole_value % 10;
			whole_value /= 10;
		} while (whole_value);

		if (is_negative)
		{
			*--begin = '-';
		}

		FastSigned fraction_value = (mantissa << whole_bits) & Float::ImplicitMantissaMask;

		if (Y_LIKELY(fraction_value && max_fraction_digits))
		{
			*end++ = '.';

			FastSigned fraction_limit = (10 + 3 * fraction_bits) / 10;

			char *fraction_end;

			if (max_fraction_digits > 0 && max_fraction_digits < fraction_limit)
			{
				fraction_end = end + max_fraction_digits;
			}
			else
			{
				fraction_end = end + fraction_limit;
			}

			FastSigned next_value = fraction_value * 10;

			do
			{
				*end++ = '0' + (next_value >> (Float::MantissaBits + 1));
				fraction_value = next_value & Float::ImplicitMantissaMask;
			} while (fraction_value && end < fraction_end);

			// TODO: Use the next digit to decide whether we should round up or down.
		}

		return StaticString(begin, end - begin);
	}

	Y_ASSERT(false);

	return StaticString();
}

} // namespace

String &String::append_dec(float value) // NOTE: Terrible terrible implementation.
{
	char buffer[32];

#if 0 // TODO: Utilize the advanced float to string conversion when it's ready.

	return append(float_to_string(buffer, value));

#else

	int size;

	sprintf(buffer, "%f%n", static_cast<double>(value), &size);
	return append(buffer, size);

#endif
}

String &String::append_dec(double value) // NOTE: Terrible terrible implementation.
{
	char buffer[32];
	int  size;

	sprintf(buffer, "%f%n", value, &size);
	return append(buffer, size);
}

String &String::clear()
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

void String::insert(const StaticString &text, size_t index)
{
	if (index > _size)
	{
		return;
	}

	size_t buffer_size = _size + text.size() + 1;

	if (_buffer_size)
	{
		grow(buffer_size);
		if (index < _size)
		{
			memmove(_text + index + text.size(), _text + index, _size - index);
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
			memcpy(_text + index + text.size(), old_text + index, _size - index);
		}
	}

	memcpy(_text + index, text.text(), text.size());
	_size += text.size();
	_text[_size] = '\0';
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

void String::remove(size_t index, size_t size)
{
	if (!size || index >= _size)
	{
		return;
	}

	size = min(size, _size - index);

	size_t *references = nullptr;

	if (_buffer_size)
	{
		references = reinterpret_cast<size_t *>(_text) - 1;

		if (*references == 1)
		{
			_size -= size;
			memmove(_text + index, _text + index + size, _size - index);
			_text[_size] = '\0';
			return;
		}
	}

	char *old_text = init(size + 1);

	if (index)
	{
		memcpy(_text, old_text, index);
	}

	_size -= size;
	memcpy(_text + index, old_text + index + size, _size - index);
	_text[_size] = '\0';

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

void String::resize(size_t size)
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
	}

	_text[size] = '\0';
	_size = size;
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

String &String::set(const char *text)
{
	return set(text, strlen(text));
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

String &String::swap(String *string)
{
	char *text = _text;
	size_t size = _size;
	size_t buffer_size = _buffer_size;
	Allocator *allocator = _allocator;

	_text = string->_text;
	_size = string->_size;
	_buffer_size = string->_buffer_size;
	_allocator = string->_allocator;

	string->_text = text;
	string->_size = size;
	string->_buffer_size = buffer_size;
	string->_allocator = allocator;

	return *this;
}

String &String::swap(String &&string)
{
	char *text = _text;
	size_t size = _size;
	size_t buffer_size = _buffer_size;
	Allocator *allocator = _allocator;

	_text = string._text;
	_size = string._size;
	_buffer_size = string._buffer_size;
	_allocator = string._allocator;

	string._text = text;
	string._size = size;
	string._buffer_size = buffer_size;
	string._allocator = allocator;

	return *this;
}

String &String::trim()
{
	StaticString trimmed_string = this->trimmed();

	size_t buffer_size = trimmed_string.size() + 1;

	if (_buffer_size)
	{
		grow(buffer_size);
	}
	else
	{
		init(buffer_size);
	}

	memmove(_text, trimmed_string.text(), trimmed_string.size());
	_text[trimmed_string.size()] = '\0';
	_size = trimmed_string.size();

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

	if (!_allocator) // NOTE: Rely on this at your own risk.
	{
		return;
	}

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

	if (!_allocator) // NOTE: Rely on this at your own risk.
	{
		return _text;
	}

	size_t adjusted_size = max<size_t>(buffer_size, StringReserve);
	size_t *pointer = static_cast<size_t *>(_allocator->allocate(sizeof(size_t) + adjusted_size));
	*pointer = 1;
	char *old_text = _text;
	_text = reinterpret_cast<char *>(pointer + 1);
	_buffer_size = adjusted_size;
	return old_text;
}

} // namespace Yttrium
