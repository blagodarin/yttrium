//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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
