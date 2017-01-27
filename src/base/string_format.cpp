#include <yttrium/string_format.h>

#include <yttrium/string.h>

#include <array>
#include <cassert>
#include <cstring>
#include <limits>

namespace Yttrium
{
	String& operator<<(String& string, const StaticString& value)
	{
		const auto old_size = string.size();
		string.resize(old_size + value.size());
		::memcpy(string.text() + old_size, value.text(), value.size());
		return string;
	}

	String& operator<<(String& string, Format<char>&& f)
	{
		const auto old_size = string.size();
		string.resize(old_size + f.count);
		::memset(string.text() + old_size, f.value, f.count);
		return string;
	}

	String& operator<<(String& string, Format<intmax_t>&& f)
	{
		uintmax_t uvalue;
		if (f.value >= 0)
		{
			uvalue = f.value;
		}
		else
		{
			uvalue = -f.value;
			--f.width;
		}

		std::array<char, std::numeric_limits<intmax_t>::digits10 + 2> buffer;

		auto i = buffer.size();
		do
		{
			buffer[--i] = uvalue % 10 + '0';
			uvalue /= 10;
			--f.width;
		} while (uvalue);

		if (f.value < 0)
			string << '-';
		if (f.width > 0)
			string << repeat('0', f.width);
		return string << StaticString(&buffer[i], buffer.size() - i);
	}

	String& operator<<(String& string, Format<uintmax_t>&& f)
	{
		std::array<char, std::numeric_limits<intmax_t>::digits10 + 1> buffer;

		auto i = buffer.size();
		do
		{
			buffer[--i] = f.value % 10 + '0';
			f.value /= 10;
			--f.width;
		} while (f.value);

		if (f.width > 0)
			string << repeat('0', f.width);
		return string << StaticString(&buffer[i], buffer.size() - i);
	}

	String& operator<<(String& string, float value)
	{
		return string << double{value};
	}

	String& operator<<(String& string, double value)
	{
		std::array<char, 32> buffer;
		return string << StaticString(buffer.data(), ::snprintf(buffer.data(), buffer.size(), "%g", value));
	}
}
