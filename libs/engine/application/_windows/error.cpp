//
// Copyright 2019 Sergei Blagodarin
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

#include "error.h"

#include <yttrium/exceptions.h>
#include <yttrium/utils/string.h>
#include "../../../core/utils/memory.h"

#include <windows.h>

#ifndef NDEBUG
#	include <iostream>
#endif

namespace Yttrium
{
	std::string error_to_string(std::uint32_t code, std::string_view fallback_message)
	{
		char* buffer = nullptr;
		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<char*>(&buffer), 0, nullptr);
		if (buffer)
		{
			auto size = std::strlen(buffer);
			if (size > 0 && buffer[size - 1] == '\n')
			{
				--size;
				if (size > 0 && buffer[size - 1] == '\r')
					--size;
			}
			UniquePtr<char[], ::LocalFree> buffer_ptr{ buffer };
			return make_string("(0x", Hex32{ code }, ") ", std::string_view{ buffer, size });
		}
		if (!fallback_message.empty())
			return make_string("(0x", Hex32{ code }, ") ", fallback_message, '.');
		return make_string("(0x", Hex32{ code }, ").");
	}

	void print_last_error(std::string_view function) noexcept
	{
		std::ignore = function;
#ifndef NDEBUG
		if (const auto error = ::GetLastError(); error != ERROR_SUCCESS)
		{
			try
			{
				std::cerr << std::string{ function } << " failed: " << error_to_string(error) << '\n';
			}
			catch (const std::bad_alloc&)
			{
			}
		}
#endif
	}

	void throw_last_error(std::string_view function)
	{
		if (const auto error = ::GetLastError(); error != ERROR_SUCCESS)
			throw InitializationError{ function, " failed: ", error_to_string(error) };
	}
}
