#include <yttrium/static_string.h>

#include <yttrium/string.h>
#include <yttrium/string_format.h>

#include <cmath>
#include <cstring>
#include <iostream>

namespace Yttrium
{
	namespace
	{
		// The threshold for the checked integer conversions is the maximum value that
		// can be multiplied by 10 and still fit into the specified type, both positive
		// and negative.
		//  - int32_t: 0x0CCCCCCC * 10 == 0x7FFFFFF8.
		//  - int64_t: 0x0CCCCCCCCCCCCCCC * 10 == 0x7FFFFFFFFFFFFFF8.

		// We also know the last decimal digit of the minimum and maximum integer values
		// because they posess some interesting mathematical properties:
		//  1) for N > 3, (pow(2, pow(2, N)) - 1) % 10 == 5 (unsigned);
		//  2a) for N > 1, (pow(2, pow(2, N) - 1) - 1) % 10 == 7 (positive signed).
		//  2b) for N > 1, pow(2, pow(2, N) - 1) % 10 == 8 (negative signed).

		template <typename T>
		T string_to_int(const char* p, const char* end)
		{
			if (p == end)
				return 0;

			// Sign.

			bool negate_result = false;
			switch (*p)
			{
			case '-': negate_result = true; // Fallthrough.
			case '+': ++p;
			}

			// Value.

			T result = 0;
			for (; p != end && *p >= '0' && *p <= '9'; ++p)
				result = result * 10 + (*p - '0');

			return negate_result ? -result : result;
		}

		template <typename T, T threshold>
		bool string_to_int(const char* p, const char* end, T* value)
		{
			if (p == end)
				return false;

			// Sign.

			bool negate_result = false;
			switch (*p)
			{
			case '-': negate_result = true; // Fallthrough.
			case '+': ++p;
			}

			// Value.

			if (p == end || *p < '0' || *p > '9')
				return false;

			T result = 0;

			do
			{
				if (result > threshold)
					return false;

				T digit = *p++ - '0';

				if (result == threshold && digit > 7 + negate_result)
					return false;

				result = result * 10 + digit;
			} while (p != end && *p >= '0' && *p <= '9');

			if (p != end)
				return false;

			*value = negate_result ? -result : result;
			return true;
		}

		template <typename T>
		T string_to_real(const char* p, const char* end)
		{
			if (p == end)
				return 0;

			// Sign.

			bool negate_result = false;

			switch (*p)
			{
			case '-': negate_result = true; // Fallthrough.
			case '+': ++p;
			}

			// Whole.

			T result = 0;

			for (; p != end && *p >= '0' && *p <= '9'; ++p)
			{
				result = result * 10 + (*p - '0');
			}

			// Fraction.

			if (p != end && *p == '.')
			{
				T factor = 1;

				for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
				{
					result = result * 10 + (*p - '0');
					factor *= 10;
				}
				result /= factor;
			}

			// Power.

			if (p != end && (*p == 'E' || *p == 'e'))
			{
				++p;

				// Power sign.

				bool negate_power = false;

				switch (*p)
				{
				case '-': negate_power = true; // Fallthrough.
				case '+': ++p;
				}

				// Power value.

				T power = 0;

				for (; p != end && *p >= '0' && *p <= '9'; ++p)
				{
					power = power * 10 + (*p - '0');
				}

				result *= ::pow(10, (negate_power ? -power : power));
			}

			return negate_result ? -result : result;
		}

		template <typename T>
		bool string_to_real(const char* p, const char* end, T* value)
		{
			if (p == end)
				return false;

			// Sign.

			bool negate_result = false;

			switch (*p)
			{
			case '-': negate_result = true; // Fallthrough.
			case '+': ++p;
			}

			// Whole.

			if (p == end || *p < '0' || *p > '9')
			{
				return false;
			}

			T result = 0;

			do
			{
				result = result * 10 + (*p++ - '0');
			} while (p != end && *p >= '0' && *p <= '9');

			// Fraction.

			if (p != end && *p == '.')
			{
				++p;

				if (p == end || *p < '0' || *p > '9')
				{
					return false;
				}

				T factor = 1;

				do
				{
					result = result * 10 + (*p++ - '0');
					factor *= 10;
				} while (p != end && *p >= '0' && *p <= '9');

				result /= factor;
			}

			// Power.

			if (p != end && (*p == 'E' || *p == 'e'))
			{
				if (++p == end)
				{
					return false;
				}

				// Power sign.

				bool negate_power = false;

				switch (*p)
				{
				case '-': negate_power = true; // Fallthrough.
				case '+': ++p;
				}

				// Power value.

				if (p == end || *p < '0' || *p > '9')
				{
					return false;
				}

				T power = 0;

				do
				{
					power = power * 10 + (*p++ - '0');
				} while (p != end && *p >= '0' && *p <= '9');

				result *= ::pow(10, (negate_power ? -power : power));
			}

			if (p != end)
			{
				return false;
			}

			*value = negate_result ? -result : result;
			return true;
		}

		template <typename T>
		T string_to_uint(const char* p, const char* end)
		{
			if (p == end)
				return 0;

			// Sign.

			if (*p == '+')
				++p;

			// Value.

			T result = 0;
			for (; p != end && *p >= '0' && *p <= '9'; ++p)
				result = result * 10 + *p - '0';

			return result;
		}
	}

	const char StringNull = '\0';

	StaticString::StaticString(const char* text)
		: _text(const_cast<char*>(text))
		, _size(::strlen(text))
	{
	}

	int StaticString::compare(const StaticString& string) const
	{
		if (_size < string._size)
		{
			int result = ::memcmp(_text, string._text, _size);
			return (result ? result : -1);
		}
		else if (_size > string._size)
		{
			int result = ::memcmp(_text, string._text, string._size);
			return (result ? result : 1);
		}
		else
		{
			return ::memcmp(_text, string._text, _size);
		}
	}

	size_t StaticString::count(const char* symbols) const
	{
		size_t result = 0;

		const auto end = _text + _size;

		for (auto i = _text; i != end; ++i)
		{
			for (auto j = symbols; *j; ++j)
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

	String StaticString::escaped(const char* symbols, char with, Allocator* allocator) const
	{
		String result(_size, allocator); // Best case assumption.

		const auto end = _text + _size;

		for (auto i = _text; i != end; ++i)
		{
			for (auto j = symbols; *j; ++j)
			{
				if (*i == *j)
				{
					result << with;
					break;
				}
			}
			result << *i;
		}

		return result;
	}

	bool StaticString::ends_with(const StaticString& substring) const
	{
		return substring._size <= _size
			&& !::memcmp(&_text[_size - substring._size], substring._text, substring._size);
	}

	StaticString StaticString::file_extension() const
	{
		const auto last_dot = find_last('.');
		return (last_dot != End) && last_dot > 0
			? StaticString(&_text[last_dot], _size - last_dot)
			: StaticString();
	}

	size_t StaticString::find_first(char symbol, size_t offset) const
	{
		if (offset < _size)
		{
			const auto end = _text + _size;
			for (auto c = _text + offset; c != end; ++c)
			{
				if (*c == symbol)
					return c - _text;
			}
		}
		return End;
	}

	size_t StaticString::find_last(char symbol, size_t offset) const
	{
		if (offset > _size)
			offset = _size;

		const auto end = _text - 1;
		for (auto c = _text + offset - 1; c != end; --c)
		{
			if (*c == symbol)
				return c - _text;
		}

		return End;
	}

	StaticString StaticString::trimmed() const
	{
		if (!_size)
			return StaticString();

		auto left = _text;
		auto right = _text + _size;

		for (; left < right; ++left)
		{
			if (*left > 32) // Assuming ASCII.
				break;
		}

		if (left == right)
			return StaticString();

		--right;

		for (; left < right; --right)
		{
			if (*right > 32) // Assuming ASCII.
				break;
		}

		return StaticString(left, right - left + 1);
	}

	double StaticString::to_double() const
	{
		return string_to_real<double>(_text, _text + _size);
	}

	float StaticString::to_float() const
	{
		return string_to_real<float>(_text, _text + _size);
	}

	int32_t StaticString::to_int32() const
	{
		return string_to_int<int32_t>(_text, _text + _size);
	}

	int64_t StaticString::to_int64() const
	{
		return string_to_int<int64_t>(_text, _text + _size);
	}

	bool StaticString::to_number(int32_t* value) const
	{
		return string_to_int<int32_t, INT32_C(0x0CCCCCCC)>(_text, _text + _size, value);
	}

	bool StaticString::to_number(float* value) const
	{
		return string_to_real(_text, _text + _size, value);
	}

	bool StaticString::to_number(double* value) const
	{
		return string_to_real(_text, _text + _size, value);
	}

	double StaticString::to_time() const
	{
		if (!_size)
			return 0;

		auto p = _text;
		const auto end = _text + _size;

		// Sign.

		bool negate_result = false;

		switch (*p)
		{
		case '-': negate_result = true; // Fallthrough.
		case '+': ++p;
		}

		// The first part (hours, minutes or seconds).

		double result = 0;

		for (; p != end && *p >= '0' && *p <= '9'; ++p)
		{
			result = result * 10 + *p - '0';
		}

		// The second part, if any (minutes or seconds).

		if (p != end && *p == ':')
		{
			double mins_or_secs = 0;

			for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
			{
				mins_or_secs = mins_or_secs * 10 + *p - '0';
			}
			result = result * 60 + mins_or_secs;
		}

		// The third part, if any (seconds).

		if (p != end && *p == ':')
		{
			double secs = 0;

			for (++p; *p >= '0' && *p <= '9'; ++p)
			{
				secs = secs * 10 + *p - '0';
			}
			result = result * 60 + secs;
		}

		// Fractions of seconds.

		if (p != end && *p == '.')
		{
			double factor = 1;

			for (++p; *p >= '0' && *p <= '9'; ++p)
			{
				result = result * 10 + *p - '0';
				factor *= 10;
			}
			result /= factor;
		}

		return negate_result ? -result : result;
	}

	uint32_t StaticString::to_uint32() const
	{
		return string_to_uint<uint32_t>(_text, _text + _size);
	}

	uint64_t StaticString::to_uint64() const
	{
		return string_to_uint<uint64_t>(_text, _text + _size);
	}

	bool operator==(const StaticString& lhs, const StaticString& rhs)
	{
		return lhs.size() == rhs.size() && !::memcmp(lhs.text(), rhs.text(), lhs.size());
	}

	bool operator!=(const StaticString& lhs, const StaticString& rhs)
	{
		return lhs.size() != rhs.size() || ::memcmp(lhs.text(), rhs.text(), lhs.size());
	}

	std::ostream& operator<<(std::ostream& stream, const StaticString& string)
	{
		stream.write(string.text(), string.size());
		return stream;
	}
}
