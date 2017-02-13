/// \file
/// \brief

#ifndef _include_yttrium_string_utils_h_
#define _include_yttrium_string_utils_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	namespace detail
	{
		Y_API void write_string(std::string&, intmax_t);
		Y_API void write_string(std::string&, uintmax_t);
		Y_API void write_string(std::string&, double);

		inline void write_string(std::string& string, char value) { string.append(1, value); }
		inline void write_string(std::string& string, const char* value) { string.append(value); }
		inline void write_string(std::string& string, const std::string& value) { string.append(value); }
		inline void write_string(std::string& string, const StaticString& value) { string.append(value.text(), value.size()); }

		inline void write_string(std::string& string, int value) { write_string(string, intmax_t{ value }); }
		inline void write_string(std::string& string, unsigned value) { write_string(string, uintmax_t{ value }); }

		inline void write_string(std::string& string, float value) { write_string(string, double{ value }); }
	}

	inline void write_string(std::string&) {}

	//
	template <typename T, typename... Args>
	void write_string(std::string& string, T&& value, Args&&... args)
	{
		detail::write_string(string, value);
		write_string(string, std::forward<Args>(args)...);
	}
}

#endif
