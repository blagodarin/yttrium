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

#include "error.h"

#include <yttrium/exceptions.h>
#include <yttrium/logger.h>
#include <yttrium/memory/smart_ptr.h>
#include <yttrium/utils/string.h>

#include <array>

#include <windows.h>

namespace
{
	Yt::SmartPtr<char, ::LocalFree> windows_error_description(unsigned long code)
	{
		Yt::SmartPtr<char, ::LocalFree> buffer;
		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), buffer.out_as<char*>(), 0, nullptr);
		if (buffer)
		{
			auto size = std::strlen(buffer);
			if (size > 0 && buffer[size - 1] == '\n')
			{
				--size;
				if (size > 0 && buffer[size - 1] == '\r')
					--size;
				buffer[size] = '\0';
			}
		}
		return buffer;
	}
}

namespace Yt
{
	std::string error_to_string(unsigned long code, std::string_view fallback_message)
	{
		if (const auto description = ::windows_error_description(code))
			return make_string("[0x", Hex32{ code }, "] ", std::string_view{ description });
		if (!fallback_message.empty())
			return make_string("[0x", Hex32{ code }, "] ", fallback_message, '.');
		return make_string("[0x", Hex32{ code }, "].");
	}

	void log_last_error(const char* function) noexcept
	{
		if (const auto error = ::GetLastError(); error != ERROR_SUCCESS)
		{
			std::array<char, Logger::MaxMessageSize + 1> buffer;
			if (const auto description = ::windows_error_description(error))
				std::snprintf(buffer.data(), buffer.size(), "[%s] (0x%08X) %s", function, static_cast<unsigned>(error), description.get());
			else
				std::snprintf(buffer.data(), buffer.size(), "[%s] (0x%08X)", function, static_cast<unsigned>(error));
			Logger::write(buffer.data());
		}
	}

	void throw_last_error(std::string_view function)
	{
		if (const auto error = ::GetLastError(); error != ERROR_SUCCESS)
			throw InitializationError{ function, " failed: ", error_to_string(error) };
	}
}
