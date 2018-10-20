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
}
