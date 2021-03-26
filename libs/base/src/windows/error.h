// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace Yt
{
	std::string error_to_string(unsigned long, std::string_view fallback_message = {});
	void log_error(const char* function, unsigned long error) noexcept;
	void log_last_error(const char* function) noexcept;
	void throw_last_error(std::string_view function);
}
