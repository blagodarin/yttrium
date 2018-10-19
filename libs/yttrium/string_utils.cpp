#include <yttrium/string_utils.h>

#include <array>
#include <charconv>
#include <limits>

namespace Yttrium
{
	namespace detail
	{
		void append_to(std::string& string, long long value)
		{
			std::array<char, std::numeric_limits<long long>::digits10 + 2> buffer; // Extra chars for a "lossy" digit and a sign.
			auto [p, e] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
			string.append(buffer.data(), p);
		}

		void append_to(std::string& string, unsigned long long value)
		{
			std::array<char, std::numeric_limits<unsigned long long>::digits10 + 1> buffer; // Extra char for a "lossy" digit.
			auto [p, e] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
			string.append(buffer.data(), p);
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
			int result = 0;
			std::from_chars(string.data(), string.data() + string.size(), result);
			return result;
		}

		int32_t to_int32(std::string_view string) noexcept
		{
			int32_t result = 0;
			std::from_chars(string.data(), string.data() + string.size(), result);
			return result;
		}

		int64_t to_int64(std::string_view string) noexcept
		{
			int64_t result = 0;
			std::from_chars(string.data(), string.data() + string.size(), result);
			return result;
		}

		uint32_t to_uint32(std::string_view string) noexcept
		{
			uint32_t result = 0;
			std::from_chars(string.data(), string.data() + string.size(), result);
			return result;
		}

		uint64_t to_uint64(std::string_view string) noexcept
		{
			uint64_t result = 0;
			std::from_chars(string.data(), string.data() + string.size(), result);
			return result;
		}

		float to_float(std::string_view string) noexcept
		{
			float result = 0.f;
			std::from_chars(string.data(), string.data() + string.size(), result);
			return result;
		}

		double to_double(std::string_view string) noexcept
		{
			double result = 0.f;
			std::from_chars(string.data(), string.data() + string.size(), result);
			return result;
		}

		bool to_number(std::string_view string, int32_t& value) noexcept
		{
			return std::errc{} == std::from_chars(string.data(), string.data() + string.size(), value).ec;
		}

		bool to_number(std::string_view string, uint32_t& value) noexcept
		{
			return std::errc{} == std::from_chars(string.data(), string.data() + string.size(), value).ec;
		}

		bool to_number(std::string_view string, float& value) noexcept
		{
			return std::errc{} == std::from_chars(string.data(), string.data() + string.size(), value).ec;
		}

		bool to_number(std::string_view string, double& value) noexcept
		{
			return std::errc{} == std::from_chars(string.data(), string.data() + string.size(), value).ec;
		}

		int to_time(std::string_view string) noexcept
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

			int result = 0;
			for (; p != end && *p >= '0' && *p <= '9'; ++p)
				result = result * 10 + *p - '0';

			if (p != end && *p == ':')
			{
				int minutes_or_seconds = 0;
				for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
					minutes_or_seconds = minutes_or_seconds * 10 + *p - '0';
				result = result * 60 + minutes_or_seconds;
			}

			if (p != end && *p == ':')
			{
				int seconds = 0;
				for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
					seconds = seconds * 10 + *p - '0';
				result = result * 60 + seconds;
			}

			result *= 1000;

			if (p != end && *p == '.')
			{
				int multiplier = 100;
				for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
				{
					result += (*p - '0') * multiplier;
					multiplier /= 10;
				}
			}

			return negative ? -result : result;
		}
	}
}
