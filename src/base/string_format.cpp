#include <yttrium/string_format.h>

#include <yttrium/date_time.h>
#include <yttrium/string.h>
#include "ieee_float.h"

#include <array>
#include <cassert>
#include <cstring>
#include <limits>

namespace Yttrium
{
	namespace
	{
		const auto negative_infinity = "-INF"_s;
		const auto positive_infinity = "INF"_s;
		const auto negative_nan = "-NAN"_s;
		const auto positive_nan = "NAN"_s;
		const auto negative_zero = "-0"_s;
		const auto positive_zero = "0"_s;

		template <typename T>
		StaticString float_to_string(char* buffer, T value, int max_fraction_digits = -1)
		{
			// NOTE: This should provide exact float-to-string-to-float conversions.
			// NOTE: This won't work for non-IEEE floats and may not work on big endian architectures.

			typedef IeeeFloat<T> Float;

			typedef typename Float::FastSigned   FastSigned;
			typedef typename Float::FastUnsigned FastUnsigned;
			typedef typename Float::Union        Union;

			Union raw_value;

			raw_value.f = value;

			bool is_negative = raw_value.i & Float::SignMask;

			FastSigned exponent = (raw_value.i & Float::ExponentMask) >> Float::MantissaBits;
			FastUnsigned mantissa = raw_value.i & Float::MantissaMask;

			if (!exponent)
			{
				if (!mantissa)
					return is_negative ? negative_zero : positive_zero;

				// Base 2 logarithm calculation trick based on a Sean Eron Anderson snippet:
				// http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogIEEE64Float

				Union u;

				u.i = mantissa | ((Float::ExponentBias + Float::MantissaBits) << Float::MantissaBits);
				u.f -= Float::ImplicitOne;

				FastSigned shift = (Float::ExponentBias + Float::MantissaBits) - (u.i >> Float::MantissaBits);

				mantissa <<= shift;
				exponent = Float::DenormalizedExponent - shift;
			}
			else if (exponent == Float::BiasedInfinityExponent)
			{
				if (!mantissa)
					return is_negative ? negative_infinity : positive_infinity;
				else
					return is_negative ? negative_nan : positive_nan;
			}
			else
			{
				exponent -= Float::ExponentBias;
				mantissa |= Float::ImplicitOne;
			}

			// At this point, the absolute value is (mantissa / 0x800000) * pow(2, exponent),
			// with [0x800000, 0xFFFFFF] mantissa range, and [-147, 127] exponent range.

			if (exponent > Float::MantissaBits) // The precision exceeds 1.
			{
				// TODO: Implement.
			}
			else if (exponent < -1) // The absolute value is in (0; 0.5) range.
			{
				// TODO: Implement.
			}
			else
			{
				FastSigned whole_bits = exponent + 1;
				FastSigned fraction_bits = Float::MantissaBits - exponent;

				FastSigned whole_value = mantissa >> fraction_bits;

				auto begin = buffer + (10 + 3 * whole_bits) / 10 + 1;
				auto end = begin;

				do
				{
					*--begin = '0' + whole_value % 10;
					whole_value /= 10;
				} while (whole_value);

				if (is_negative)
					*--begin = '-';

				FastSigned fraction_value = (mantissa << whole_bits) & Float::ImplicitMantissaMask;

				if (fraction_value && max_fraction_digits)
				{
					*end++ = '.';

					FastSigned fraction_limit = (10 + 3 * fraction_bits) / 10;

					auto fraction_end = end
						+ (max_fraction_digits > 0 && max_fraction_digits < fraction_limit
							? max_fraction_digits
							: fraction_limit);

					char last_digit;

					do
					{
						FastSigned next_value = fraction_value * 10;
						last_digit = '0' + (next_value >> (Float::MantissaBits + 1));
						*end++ = last_digit;
						fraction_value = next_value & Float::ImplicitMantissaMask;
					} while (fraction_value && end < fraction_end);

					if (fraction_value)
					{
						char extra_digit = '0' + ((fraction_value * 10) >> (Float::MantissaBits + 1));

						if (extra_digit > '5' || (extra_digit == '5' && ((last_digit - '0') & 1)))
						{
							++last_digit;
							while (last_digit > '9')
							{
								--end;
								last_digit = *(end - 1);
								if (last_digit == '.')
								{
									last_digit = '0';
									++end;
									break;
								}
								else
								{
									++last_digit;
								}
							}
							*(end - 1) = last_digit;
						}
					}
				}

				return StaticString(begin, end - begin);
			}

			assert(false);

			return StaticString();
		}
	}

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
	#if 0 // TODO: Utilize the advanced float to string conversion when it's ready.
		std::array<char, 32> buffer;
		return string << float_to_string(buffer.data(), value);
	#else
		return string << double{value};
	#endif
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
