//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#pragma once

#include <yttrium/api.h>

#include <string>

namespace Yt
{
	Y_CORE_API std::string error_to_string(unsigned long, std::string_view fallback_message = {});
	Y_CORE_API void log_error(const char* function, unsigned long error) noexcept;
	Y_CORE_API void log_last_error(const char* function) noexcept;
	Y_CORE_API void throw_last_error(std::string_view function);
}
