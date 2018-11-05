#ifndef _libs_yttrium_utils_string_h_
#define _libs_yttrium_utils_string_h_

#include <yttrium/api.h>

#include <string>

namespace Yttrium
{
	inline bool ends_with(std::string_view string, std::string_view with) noexcept
	{
		return string.size() >= with.size() && string.substr(string.size() - with.size()) == with;
	}

	Y_CORE_API bool from_chars(std::string_view, int32_t&) noexcept;
	Y_CORE_API bool from_chars(std::string_view, uint32_t&) noexcept;
	Y_CORE_API bool from_chars(std::string_view, float&) noexcept;
	Y_CORE_API bool from_chars(std::string_view, double&) noexcept;

	Y_CORE_API int time_from_chars(std::string_view) noexcept;
}

#endif
