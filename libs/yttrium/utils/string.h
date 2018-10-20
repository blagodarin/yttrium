#pragma once

#include <string_view>

namespace Yttrium
{
	namespace strings
	{
		inline bool ends_with(std::string_view string, std::string_view with) noexcept
		{
			return string.size() >= with.size() && string.substr(string.size() - with.size()) == with;
		}

		bool to_number(std::string_view, int32_t&) noexcept;
		bool to_number(std::string_view, uint32_t&) noexcept;
		bool to_number(std::string_view, float&) noexcept;
		bool to_number(std::string_view, double&) noexcept;

		int to_time(std::string_view) noexcept;
	}
}
