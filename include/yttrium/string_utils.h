/// \file
/// \brief

#ifndef _include_yttrium_string_utils_h_
#define _include_yttrium_string_utils_h_

#include <yttrium/api.h>

#include <string>

namespace Yttrium
{
	namespace detail
	{
		inline void append_to(std::string& string, char value) { string.append(1, value); }
		inline void append_to(std::string& string, std::string_view value) { string.append(value); }

		Y_API void append_to(std::string&, long long);
		inline void append_to(std::string& string, int value) { append_to(string, static_cast<long long>(value)); }
		inline void append_to(std::string& string, long value) { append_to(string, static_cast<long long>(value)); }

		Y_API void append_to(std::string&, unsigned long long);
		inline void append_to(std::string& string, unsigned long value) { append_to(string, static_cast<unsigned long long>(value)); }
		inline void append_to(std::string& string, unsigned value) { append_to(string, static_cast<unsigned long long>(value)); }

		Y_API void append_to(std::string&, double);
		inline void append_to(std::string& string, float value) { append_to(string, double{value}); }
	}

	inline void append_to(std::string&) {}

	///
	template <typename T, typename... Args>
	void append_to(std::string& string, T&& value, Args&&... args)
	{
		detail::append_to(string, value);
		append_to(string, std::forward<Args>(args)...);
	}

	///
	template <typename... Args>
	std::string make_string(Args&&... args)
	{
		std::string string;
		append_to(string, std::forward<Args>(args)...);
		return string;
	}

	///
	namespace strings
	{
		///
		inline bool ends_with(std::string_view string, std::string_view with) noexcept
		{
			return string.size() >= with.size() && string.substr(string.size() - with.size()) == with;
		}

		/// Converts the string to signed decimal integer.
		/// The string must be in form `[+|-]d{d}`.
		Y_API int to_int(std::string_view) noexcept;
		Y_API int32_t to_int32(std::string_view) noexcept;
		Y_API int64_t to_int64(std::string_view) noexcept;

		/// Converts the string to unsigned decimal integer.
		/// The string must be in form `[+]d{d}`.
		Y_API uint32_t to_uint32(std::string_view) noexcept;
		Y_API uint64_t to_uint64(std::string_view) noexcept;

		/// Converts the string to decimal floating point value.
		/// The string must be in form `[+|-]d{d}[.d{d}][(e|E)[+|-]d{d}]`.
		Y_API float to_float(std::string_view) noexcept;
		Y_API double to_double(std::string_view) noexcept;

		/// Tries to convert the string to decimal value.
		/// Returns `false` if the string doesn't match the numeric format.
		Y_API bool to_number(std::string_view, int32_t&) noexcept;
		Y_API bool to_number(std::string_view, uint32_t&) noexcept;
		Y_API bool to_number(std::string_view, float&) noexcept;
		Y_API bool to_number(std::string_view, double&) noexcept;

		/// Interprets the string as a duration, returning the corresponding number of milliseconds.
		/// The string must be in form `[+|-][[HH:]MM:]SS[.{Z}]`.
		Y_API int to_time(std::string_view) noexcept;
	}
}

#endif
