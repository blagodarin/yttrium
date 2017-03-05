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
		inline void append_to(std::string& string, const char* value) { string.append(value); }
		inline void append_to(std::string& string, const std::string& value) { string.append(value); }

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
}

#endif
