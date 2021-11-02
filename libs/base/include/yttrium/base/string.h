// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <string_view>

namespace Yt
{
	bool from_chars(std::string_view, int32_t&) noexcept;
	bool from_chars(std::string_view, uint32_t&) noexcept;
	bool from_chars(std::string_view, float&) noexcept;
	bool from_chars(std::string_view, double&) noexcept;
}
