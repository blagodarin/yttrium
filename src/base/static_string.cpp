#include <cmath>   // pow
#include <cstring> // memcmp

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

size_t StaticString::count(const char *symbols) const throw()
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

double StaticString::to_double() const throw()
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

int32_t StaticString::to_int32() const throw()
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

int64_t StaticString::to_int64() const throw()
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

bool StaticString::to_number(int32_t &value) const throw()
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

bool StaticString::to_number(double &value) const throw()
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

double StaticString::to_time() const throw()
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

uint32_t StaticString::to_uint32() const throw()
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

uint64_t StaticString::to_uint64() const throw()
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

const size_t StaticString::End;

const char StaticString::Null;

} // namespace Yttrium
