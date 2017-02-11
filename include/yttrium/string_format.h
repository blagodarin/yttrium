/// \file
/// \brief %String formatting.

#ifndef _include_yttrium_string_format_h_
#define _include_yttrium_string_format_h_

#include <yttrium/static_string.h>

#include <type_traits>
#include <utility>

namespace Yttrium
{
	class String;

	template <typename> class Format;

	///
	inline Format<char> repeat(char value, size_t count);

	/// Produces a decimal formatting for a signed integral \a value.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value>>
	Format<intmax_t> dec(T value, int width = 0);

	/// Produces a decimal formatting for a unsigned integral \a value.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value>>
	Format<uintmax_t> dec(T value, int width = 0);

	///
	inline String& operator<<(String&, char);

	///
	Y_API String& operator<<(String&, const StaticString&);

	///
	inline String& operator<<(String&, const std::string&);

	///
	inline String& operator<<(String&, const char*);

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

	template <>
	class Format<char>
	{
		friend Format repeat(char, size_t);
		friend Y_API String& operator<<(String&, Format<char>&&);
		char value;
		size_t count;
		Format(char value, size_t count) : value(value), count(count) {}
	};

	Format<char> repeat(char value, size_t count)
	{
		return {value, count};
	}

	template <>
	class Format<intmax_t>
	{
		template <typename T, typename> friend Format dec(T, int);
		friend Y_API String& operator<<(String&, Format<intmax_t>&&);
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
		friend Y_API String& operator<<(String&, Format<uintmax_t>&&);
		uintmax_t value;
		int width;
		Format(uintmax_t value, int width) : value(value), width(width) {}
	};

	template <typename T, typename>
	Format<uintmax_t> dec(T value, int width)
	{
		return {value, width};
	}

	inline String& operator<<(String& string, char value)
	{
		return string << repeat(value, 1);
	}

	inline String& operator<<(String& string, const std::string& value)
	{
		return string << StaticString(value);
	}

	inline String& operator<<(String& string, const char* value)
	{
		return string << StaticString(value);
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
