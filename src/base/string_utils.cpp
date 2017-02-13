#include <yttrium/string_utils.h>

#include <array>
#include <limits>

namespace Yttrium
{
	namespace detail
	{
		void write_string(std::string& string, intmax_t value)
		{
			std::array<char, std::numeric_limits<intmax_t>::digits10 + 2> buffer; // Extra chars for a "lossy" digit and a sign.
			uintmax_t uvalue = value >= 0 ? value : -value;
			auto i = buffer.size();
			do
			{
				buffer[--i] = uvalue % 10 + '0';
				uvalue /= 10;
			} while (uvalue);
			if (value < 0)
				buffer[--i] = '-';
			string.append(&buffer[i], buffer.size() - i);
		}

		void write_string(std::string& string, uintmax_t value)
		{
			std::array<char, std::numeric_limits<uintmax_t>::digits10 + 1> buffer; // Extra char for a "lossy" digit.
			auto i = buffer.size();
			do
			{
				buffer[--i] = value % 10 + '0';
				value /= 10;
			} while (value > 0);
			string.append(&buffer[i], buffer.size() - i);
		}

		void write_string(std::string& string, double value)
		{
			std::array<char, 32> buffer;
			string.append(buffer.data(), std::snprintf(buffer.data(), buffer.size(), "%g", value));
		}
	}
}
