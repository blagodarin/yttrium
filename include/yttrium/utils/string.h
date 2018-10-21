#ifndef _include_yttrium_utils_string_h_
#define _include_yttrium_utils_string_h_

#include <yttrium/api.h>

#include <string>

namespace Yttrium
{
	Y_API void _append_to(std::string&, long long);
	Y_API void _append_to(std::string&, unsigned long long);
	Y_API void _append_to(std::string&, double);

	inline void _append_to(std::string& string, char value) { string.append(1, value); }
	inline void _append_to(std::string& string, int value) { _append_to(string, static_cast<long long>(value)); }
	inline void _append_to(std::string& string, long value) { _append_to(string, static_cast<long long>(value)); }
	inline void _append_to(std::string& string, unsigned value) { _append_to(string, static_cast<unsigned long long>(value)); }
	inline void _append_to(std::string& string, unsigned long value) { _append_to(string, static_cast<unsigned long long>(value)); }
	inline void _append_to(std::string& string, float value) { _append_to(string, static_cast<double>(value)); }
	inline void _append_to(std::string& string, std::string_view value) { string.append(value); }

	inline void append_to(std::string&) {}

	///
	template <typename T, typename... Args>
	void append_to(std::string& string, T&& value, Args&&... args)
	{
		_append_to(string, value);
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