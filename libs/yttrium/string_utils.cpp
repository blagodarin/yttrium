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
			auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
			string.append(buffer.data(), ptr);
		}

		void append_to(std::string& string, unsigned long long value)
		{
			std::array<char, std::numeric_limits<unsigned long long>::digits10 + 1> buffer; // Extra char for a "lossy" digit.
			auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
			string.append(buffer.data(), ptr);
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

		bool to_number(std::string_view string, int32_t& value) noexcept
		{
			const auto end = string.data() + string.size();
			auto [ptr, ec] = std::from_chars(string.data(), string.data() + string.size(), value);
			return ptr == end && ec == std::errc{};
		}

		bool to_number(std::string_view string, uint32_t& value) noexcept
		{
			const auto end = string.data() + string.size();
			auto [ptr, ec] = std::from_chars(string.data(), string.data() + string.size(), value);
			return ptr == end && ec == std::errc{};
		}

		bool to_number(std::string_view string, float& value) noexcept
		{
			const auto end = string.data() + string.size();
			auto [ptr, ec] = std::from_chars(string.data(), string.data() + string.size(), value);
			return ptr == end && ec == std::errc{};
		}

		bool to_number(std::string_view string, double& value) noexcept
		{
			const auto end = string.data() + string.size();
			auto [ptr, ec] = std::from_chars(string.data(), string.data() + string.size(), value);
			return ptr == end && ec == std::errc{};
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
