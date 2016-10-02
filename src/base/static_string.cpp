#include <yttrium/static_string.h>

#include <yttrium/string.h>
#include <yttrium/string_format.h>

#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>

namespace
{
	template <typename T>
	T string_to_int(const char* p, const char* end)
	{
		if (p == end)
			return 0;

		bool negative = false;
		switch (*p)
		{
		case '-': negative = true;
		case '+': ++p;
		}

		T result = 0;
		for (; p != end && *p >= '0' && *p <= '9'; ++p)
			result = result * 10 + (*p - '0');

		return negative ? -result : result;
	}

	template <typename T>
	bool string_to_int(const char* p, const char* end, T& value)
	{
		if (p == end)
			return false;

		bool negative = false;
		switch (*p)
		{
		case '-': negative = true;
		case '+': ++p;
		}

		if (p == end || *p < '0' || *p > '9')
			return false;

		T result = 0;
		do
		{
			if (result > std::numeric_limits<T>::max() / 10)
				return false;

			T digit = *p++ - '0';

			// The maximum value for an N-bit signed integral number is 2^(2^N - 1) - 1.
			// For N > 1, (2^(2^N - 1) - 1) % 10 == 7.
			// The minimum value for an N-bit signed integral number is -2^(2^N - 1).
			// For N > 1, 2^(2^N - 1) % 10 == 8.
			if (result == std::numeric_limits<T>::max() / 10 && digit > 7 + negative)
				return false;

			result = result * 10 + digit;
		} while (p != end && *p >= '0' && *p <= '9');

		if (p != end)
			return false;

		value = negative ? -result : result;
		return true;
	}

	template <typename T>
	T string_to_uint(const char* p, const char* end)
	{
		if (p == end)
			return 0;

		if (*p == '+')
			++p;

		T result = 0;
		for (; p != end && *p >= '0' && *p <= '9'; ++p)
			result = result * 10 + *p - '0';

		return result;
	}

	template <typename T>
	bool string_to_uint(const char* p, const char* end, T& value)
	{
		if (p == end)
			return false;

		if (*p == '+')
			++p;

		if (p == end || *p < '0' || *p > '9')
			return false;

		T result = 0;
		do
		{
			if (result > std::numeric_limits<T>::max() / 10)
				return false;

			T digit = *p++ - '0';

			// The maximum value for an N-bit unsigned integral number is 2^(2^N) - 1.
			// For N > 3, (2^(2^N)-1) % 10 == 5.
			if (result == std::numeric_limits<T>::max() / 10 && digit > 5)
				return false;

			result = result * 10 + digit;
		} while (p != end && *p >= '0' && *p <= '9');

		if (p != end)
			return false;

		value = result;
		return true;
	}

	template <typename T>
	T string_to_float(const char* p, const char* end)
	{
		if (p == end)
			return 0;

		bool negative = false;
		switch (*p)
		{
		case '-': negative = true;
		case '+': ++p;
		}

		T result = 0;
		for (; p != end && *p >= '0' && *p <= '9'; ++p)
			result = result * 10 + (*p - '0');

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

		if (p != end && (*p == 'E' || *p == 'e'))
		{
			++p;

			bool negative_power = false;
			switch (*p)
			{
			case '-': negative_power = true;
			case '+': ++p;
			}

			T power = 0;
			for (; p != end && *p >= '0' && *p <= '9'; ++p)
				power = power * 10 + (*p - '0');

			result *= ::pow(10, negative_power ? -power : power);
		}

		return negative ? -result : result;
	}

	template <typename T>
	bool string_to_float(const char* p, const char* end, T& value)
	{
		if (p == end)
			return false;

		bool negative = false;
		switch (*p)
		{
		case '-': negative = true;
		case '+': ++p;
		}

		if (p == end || *p < '0' || *p > '9')
			return false;

		T result = 0;
		do
		{
			result = result * 10 + (*p++ - '0');
		} while (p != end && *p >= '0' && *p <= '9');

		if (p != end && *p == '.')
		{
			++p;

			if (p == end || *p < '0' || *p > '9')
				return false;

			T factor = 1;
			do
			{
				result = result * 10 + (*p++ - '0');
				factor *= 10;
			} while (p != end && *p >= '0' && *p <= '9');

			result /= factor;
		}

		if (p != end && (*p == 'E' || *p == 'e'))
		{
			if (++p == end)
				return false;

			bool negative_power = false;
			switch (*p)
			{
			case '-': negative_power = true;
			case '+': ++p;
			}

			if (p == end || *p < '0' || *p > '9')
				return false;

			T power = 0;
			do
			{
				power = power * 10 + (*p++ - '0');
			} while (p != end && *p >= '0' && *p <= '9');

			result *= ::pow(10, negative_power ? -power : power);
		}

		if (p != end)
			return false;

		value = negative ? -result : result;
		return true;
	}
}

namespace Yttrium
{
	StaticString::StaticString(const char* text)
		: _text(text ? const_cast<char*>(text) : &Null)
		, _size(::strlen(_text))
	{
	}

	bool StaticString::ends_with(const StaticString& other) const
	{
		return right(other.size()) == other;
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

	StaticString StaticString::file_name_extension() const
	{
		const auto last_dot = find_last('.');
		return last_dot != End && last_dot > 0
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

	double StaticString::to_double() const
	{
		return ::string_to_float<double>(_text, _text + _size);
	}

	float StaticString::to_float() const
	{
		return ::string_to_float<float>(_text, _text + _size);
	}

	int StaticString::to_int() const
	{
		return ::string_to_int<int>(_text, _text + _size);
	}

	int32_t StaticString::to_int32() const
	{
		return ::string_to_int<int32_t>(_text, _text + _size);
	}

	int64_t StaticString::to_int64() const
	{
		return ::string_to_int<int64_t>(_text, _text + _size);
	}

	bool StaticString::to_number(int32_t& value) const
	{
		return ::string_to_int(_text, _text + _size, value);
	}

	bool StaticString::to_number(uint32_t& value) const
	{
		return ::string_to_uint(_text, _text + _size, value);
	}

	bool StaticString::to_number(float& value) const
	{
		return ::string_to_float(_text, _text + _size, value);
	}

	bool StaticString::to_number(double& value) const
	{
		return ::string_to_float(_text, _text + _size, value);
	}

	double StaticString::to_time() const
	{
		if (!_size)
			return 0;

		auto p = _text;
		const auto end = _text + _size;

		bool negative = false;
		switch (*p)
		{
		case '-': negative = true;
		case '+': ++p;
		}

		double result = 0;
		for (; p != end && *p >= '0' && *p <= '9'; ++p)
			result = result * 10 + *p - '0';

		if (p != end && *p == ':')
		{
			double mins_or_secs = 0;
			for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
				mins_or_secs = mins_or_secs * 10 + *p - '0';
			result = result * 60 + mins_or_secs;
		}

		if (p != end && *p == ':')
		{
			double secs = 0;
			for (++p; *p >= '0' && *p <= '9'; ++p)
				secs = secs * 10 + *p - '0';
			result = result * 60 + secs;
		}

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

		return negative ? -result : result;
	}

	unsigned StaticString::to_uint() const
	{
		return ::string_to_uint<unsigned>(_text, _text + _size);
	}

	uint32_t StaticString::to_uint32() const
	{
		return ::string_to_uint<uint32_t>(_text, _text + _size);
	}

	uint64_t StaticString::to_uint64() const
	{
		return ::string_to_uint<uint64_t>(_text, _text + _size);
	}

	StaticString StaticString::trimmed() const
	{
		if (!_size)
			return {};

		auto left = _text;
		auto right = _text + _size;

		for (; left < right; ++left)
		{
			if (static_cast<unsigned char>(*left) > 32) // Assuming ASCII.
				break;
		}

		if (left == right)
			return {};

		--right;

		for (; left < right; --right)
		{
			if (static_cast<unsigned char>(*right) > 32) // Assuming ASCII.
				break;
		}

		return StaticString(left, right - left + 1);
	}

	const char StaticString::Null = '\0';

	int compare(const StaticString& a, const StaticString& b)
	{
		if (a.size() < b.size())
		{
			const auto result = ::memcmp(a.text(), b.text(), a.size());
			return result ? result : -1;
		}
		else if (a.size() > b.size())
		{
			const auto result = ::memcmp(a.text(), b.text(), b.size());
			return result ? result : 1;
		}
		else
		{
			return ::memcmp(a.text(), b.text(), a.size());
		}
	}

	bool operator==(const StaticString& a, const StaticString& b)
	{
		return a.size() == b.size() && !::memcmp(a.text(), b.text(), a.size());
	}

	bool operator!=(const StaticString& a, const StaticString& b)
	{
		return a.size() != b.size() || ::memcmp(a.text(), b.text(), a.size());
	}

	std::ostream& operator<<(std::ostream& stream, const StaticString& string)
	{
		stream.write(string.text(), string.size());
		return stream;
	}
}
