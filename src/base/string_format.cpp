#include <yttrium/string_format.h>

#include <yttrium/date_time.h>
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
			string << rep('0', f.width);
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
			string << rep('0', f.width);
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

	String& operator<<(String& string, const Format<DateTime>& f)
	{
		string.reserve(string.size() + ::strlen(f.format)); // A good estimate.
		for (auto format = f.format; *format != '\0';)
		{
			if (*format != '%')
			{
				string << *format++;
				continue;
			}
			switch (*++format)
			{
			case '%':
				string << '%';
				++format;
				break;

			case 'D':
				if (*++format == 'D')
				{
					string << dec(f.value.day, 2);
					++format;
				}
				else
					string << f.value.day;
				break;

			case 'M':
				if (*++format == 'M')
				{
					string << dec(f.value.month, 2);
					++format;
				}
				else
					string << f.value.month;
				break;

			case 'Y':
				if (*++format == 'Y')
				{
					string << dec(f.value.year, 4);
					++format;
				}
				else
					string << f.value.year;
				break;

			case 'h':
				if (*++format == 'h')
				{
					string << dec(f.value.hour, 2);
					++format;
				}
				else
					string << f.value.hour;
				break;

			case 'm':
				if (*++format == 'm')
				{
					string << dec(f.value.minute, 2);
					++format;
				}
				else
					string << f.value.minute;
				break;

			case 's':
				if (*++format == 's')
				{
					string << dec(f.value.second, 2);
					++format;
				}
				else
					string << f.value.second;
				break;

			case 'z':
				if (*++format == 'z')
				{
					string << dec(f.value.msecond, 3);
					++format;
				}
				else
					string << f.value.msecond;
				break;

			default:
				string << '%';
			}
		}
		return string;
	}
}
