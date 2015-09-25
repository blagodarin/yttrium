/// \file
/// \brief %String formatting.

#ifndef _include_yttrium_string_format_h_
#define _include_yttrium_string_format_h_

#include <yttrium/global.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace Yttrium
{
	struct DateTime;
	class StaticString;
	class String;

	template <typename> class Format;

	///
	inline Format<char> rep(char value, size_t count);

	/// Produces a decimal formatting for a signed integral \a value.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value>>
	Format<intmax_t> dec(T value, int width = 0);

	/// Produces a decimal formatting for a unsigned integral \a value.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value>>
	Format<uintmax_t> dec(T value, int width = 0);

	///
	inline Format<DateTime> print(const DateTime& value, const char* format);

	///
	Y_API String& operator<<(String&, const StaticString&);

	///
	Y_API String& operator<<(String&, Format<char>&&);

	///
	Y_API String& operator<<(String&, Format<intmax_t>&&);

	///
	Y_API String& operator<<(String&, Format<uintmax_t>&&);

	///
	Y_API String& operator<<(String&, float);

	///
	Y_API String& operator<<(String&, double);

	///
	Y_API String& operator<<(String&, const Format<DateTime>&);

	template <>
	class Format<char>
	{
		friend Format rep(char, size_t);
		friend String& operator<<(String&, Format<char>&&);
		char value;
		size_t count;
		Format(char value, size_t count) : value(value), count(count) {}
	};

	Format<char> rep(char value, size_t count)
	{
		return {value, count};
	}

	template <>
	class Format<intmax_t>
	{
		template <typename T, typename> friend Format dec(T, int);
		friend String& operator<<(String&, Format<intmax_t>&&);
		intmax_t value;
		int width;
		Format(intmax_t value, int width) : value(value), width(width) {}
	};

	template <typename T, typename>
	Format<intmax_t> dec(T value, int width)
	{
		return {value, width};
	}

	template <>
	class Format<uintmax_t>
	{
		template <typename T, typename> friend Format dec(T, int);
		friend String& operator<<(String&, Format<uintmax_t>&&);
		uintmax_t value;
		int width;
		Format(uintmax_t value, int width) : value(value), width(width) {}
	};

	template <typename T, typename>
	Format<uintmax_t> dec(T value, int width)
	{
		return {value, width};
	}

	template <>
	class Format<DateTime>
	{
		friend Format print(const DateTime&, const char*);
		friend String& operator<<(String&, const Format<DateTime>&);
		const DateTime& value;
		const char* format;
		Format(const DateTime& value, const char* format) : value(value), format(format) {}
	};

	Format<DateTime> print(const DateTime& value, const char* format)
	{
		return {value, format};
	}

	inline String& operator<<(String& string, char value)
	{
		return string << rep(value, 1);
	}

	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
	String& operator<<(String& string, T value)
	{
		return string << dec(value);
	}

	template <typename T>
	String&& operator<<(String&& string, T&& value)
	{
		return std::move(string << std::forward<T>(value));
	}
}

#endif
