// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace Yt
{
	inline bool ends_with(std::string_view string, std::string_view with) noexcept
	{
		return string.size() >= with.size() && string.substr(string.size() - with.size()) == with;
	}
}
