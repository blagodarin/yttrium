// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "error.h"

#include <yttrium/base/exceptions.h>
#include <yttrium/base/logger.h>

#include <seir_base/pointer.hpp>

#include <array>

#include <windows.h>

#include <fmt/format.h>

namespace
{
	auto windows_error_description(unsigned long code)
	{
		seir::CPtr<char, ::LocalFree> buffer;
		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<char*>(buffer.out()), 0, nullptr);
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
			return fmt::format("[0x{:8X}] {}", code, description.get());
		if (!fallback_message.empty())
			return fmt::format("[0x{:8X}] {}.", code, fallback_message);
		return fmt::format("[0x{:8X}].", code);
	}

	void log_error(const char* function, unsigned long error) noexcept
	{
		std::array<char, Logger::MaxMessageSize + 1> buffer;
		if (const auto description = ::windows_error_description(error))
			*fmt::format_to_n(buffer.data(), buffer.size() - 1, "[{}] (0x{:8X}) {}", function, error, description.get()).out = '\0';
		else
			*fmt::format_to_n(buffer.data(), buffer.size() - 1, "[{}] (0x{:8X})", function, error).out = '\0';
		Logger::write(buffer.data());
	}

	void log_last_error(const char* function) noexcept
	{
		if (const auto error = ::GetLastError(); error != ERROR_SUCCESS)
			log_error(function, error);
	}

	void throw_last_error(std::string_view function)
	{
		if (const auto error = ::GetLastError(); error != ERROR_SUCCESS)
			throw InitializationError{ fmt::format("{} failed: {}", function, error_to_string(error)) };
	}
}
