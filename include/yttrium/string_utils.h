/// \file
/// \brief

#ifndef _include_yttrium_string_utils_h_
#define _include_yttrium_string_utils_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	namespace detail
	{
		inline void write_string(std::string& string, char value) { string.append(1, value); }
		inline void write_string(std::string& string, const char* value) { string.append(value); }
		inline void write_string(std::string& string, const std::string& value) { string.append(value); }
		inline void write_string(std::string& string, const StaticString& value) { string.append(value.text(), value.size()); }

		Y_API void write_string(std::string&, long long);
		inline void write_string(std::string& string, int value) { write_string(string, static_cast<long long>(value)); }
		inline void write_string(std::string& string, long value) { write_string(string, static_cast<long long>(value)); }

		Y_API void write_string(std::string&, unsigned long long);
		inline void write_string(std::string& string, unsigned long value) { write_string(string, static_cast<unsigned long long>(value)); }
		inline void write_string(std::string& string, unsigned value) { write_string(string, static_cast<unsigned long long>(value)); }

		Y_API void write_string(std::string&, double);
		inline void write_string(std::string& string, float value) { write_string(string, double{ value }); }
	}

	inline void write_string(std::string&) {}

	///
	template <typename T, typename... Args>
	void write_string(std::string& string, T&& value, Args&&... args)
	{
		detail::write_string(string, value);
		write_string(string, std::forward<Args>(args)...);
	}

	///
	template <typename... Args>
	std::string make_string(Args&&... args)
	{
		std::string string;
		write_string(string, std::forward<Args>(args)...);
		return string;
	}
}

#endif
