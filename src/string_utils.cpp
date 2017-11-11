#include <yttrium/string_utils.h>

#include <array>
#include <cmath>
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
		case '-': negative = true; [[fallthrough]];
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
		case '-': negative = true; [[fallthrough]];
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
	T string_to_unsigned(const char* p, const char* end)
	{
		if (p == end)
			return 0;

		if (*p == '+')
			++p;

		T result = 0;
		for (; p != end && *p >= '0' && *p <= '9'; ++p)
			result = result * 10 + static_cast<T>(*p - '0');

		return result;
	}

	template <typename T>
	bool string_to_unsigned(const char* p, const char* end, T& value)
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

			const auto digit = static_cast<T>(*p++ - '0');

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
		case '-': negative = true; [[fallthrough]];
		case '+': ++p;
		}

		T result = 0;
		for (; p != end && *p >= '0' && *p <= '9'; ++p)
			result = result * 10 + static_cast<T>(*p - '0');

		if (p != end && *p == '.')
		{
			T factor = 1;
			for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
			{
				result = result * 10 + static_cast<T>(*p - '0');
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
			case '-': negative_power = true; [[fallthrough]];
			case '+': ++p;
			}

			T power = 0;
			for (; p != end && *p >= '0' && *p <= '9'; ++p)
				power = power * 10 + static_cast<T>(*p - '0');

			result *= std::pow(T{10}, negative_power ? -power : power);
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
		case '-': negative = true; [[fallthrough]];
		case '+': ++p;
		}

		if (p == end || *p < '0' || *p > '9')
			return false;

		T result = 0;
		do
		{
			result = result * 10 + static_cast<T>(*p++ - '0');
		} while (p != end && *p >= '0' && *p <= '9');

		if (p != end && *p == '.')
		{
			++p;

			if (p == end || *p < '0' || *p > '9')
				return false;

			T factor = 1;
			do
			{
				result = result * 10 + static_cast<T>(*p++ - '0');
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
			case '-': negative_power = true; [[fallthrough]];
			case '+': ++p;
			}

			if (p == end || *p < '0' || *p > '9')
				return false;

			T power = 0;
			do
			{
				power = power * 10 + static_cast<T>(*p++ - '0');
			} while (p != end && *p >= '0' && *p <= '9');

			result *= std::pow(T{10}, negative_power ? -power : power);
		}

		if (p != end)
			return false;

		value = negative ? -result : result;
		return true;
	}
}

namespace Yttrium
{
	namespace detail
	{
		void append_to(std::string& string, long long value)
		{
			std::array<char, std::numeric_limits<long long>::digits10 + 2> buffer; // Extra chars for a "lossy" digit and a sign.
			auto uvalue = static_cast<unsigned long long>(value >= 0 ? value : -value);
			auto i = buffer.size();
			do
			{
				buffer[--i] = static_cast<char>(uvalue % 10 + static_cast<unsigned char>('0'));
				uvalue /= 10;
			} while (uvalue);
			if (value < 0)
				buffer[--i] = '-';
			string.append(&buffer[i], buffer.size() - i);
		}

		void append_to(std::string& string, unsigned long long value)
		{
			std::array<char, std::numeric_limits<unsigned long long>::digits10 + 1> buffer; // Extra char for a "lossy" digit.
			auto i = buffer.size();
			do
			{
				buffer[--i] = static_cast<char>(value % 10 + static_cast<unsigned char>('0'));
				value /= 10;
			} while (value > 0);
			string.append(&buffer[i], buffer.size() - i);
		}

		void append_to(std::string& string, double value)
		{
			std::array<char, 32> buffer;
			string.append(buffer.data(), static_cast<size_t>(std::snprintf(buffer.data(), buffer.size(), "%g", value)));
		}
	}

	namespace strings
	{
		int to_int(std::string_view string) noexcept
		{
			return ::string_to_int<int>(string.data(), string.data() + string.size());
		}

		int32_t to_int32(std::string_view string) noexcept
		{
			return ::string_to_int<int32_t>(string.data(), string.data() + string.size());
		}

		int64_t to_int64(std::string_view string) noexcept
		{
			return ::string_to_int<int64_t>(string.data(), string.data() + string.size());
		}

		uint32_t to_uint32(std::string_view string) noexcept
		{
			return ::string_to_unsigned<uint32_t>(string.data(), string.data() + string.size());
		}

		uint64_t to_uint64(std::string_view string) noexcept
		{
			return ::string_to_unsigned<uint64_t>(string.data(), string.data() + string.size());
		}

		float to_float(std::string_view string) noexcept
		{
			return ::string_to_float<float>(string.data(), string.data() + string.size());
		}

		double to_double(std::string_view string) noexcept
		{
			return ::string_to_float<double>(string.data(), string.data() + string.size());
		}

		bool to_number(std::string_view string, int32_t& value) noexcept
		{
			return ::string_to_int(string.data(), string.data() + string.size(), value);
		}

		bool to_number(std::string_view string, uint32_t& value) noexcept
		{
			return ::string_to_unsigned(string.data(), string.data() + string.size(), value);
		}

		bool to_number(std::string_view string, float& value) noexcept
		{
			return ::string_to_float(string.data(), string.data() + string.size(), value);
		}

		bool to_number(std::string_view string, double& value) noexcept
		{
			return ::string_to_float(string.data(), string.data() + string.size(), value);
		}

		double to_time(std::string_view string) noexcept
		{
			if (string.empty())
				return 0;

			auto p = string.data();
			const auto end = string.data() + string.size();

			bool negative = false;
			switch (*p)
			{
			case '-': negative = true; [[fallthrough]];
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
	}
}
