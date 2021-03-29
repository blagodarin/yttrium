// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <string>

namespace Yt
{
	bool from_chars(std::string_view, int32_t&) noexcept;
	bool from_chars(std::string_view, uint32_t&) noexcept;
	bool from_chars(std::string_view, float&) noexcept;
	bool from_chars(std::string_view, double&) noexcept;

	// Replaces sequences of spaces and ASCII control characters with a single space.
	// Leading space is always removed, trailing space is removed depending on the parameter.
	void strip(std::string&, bool removeTrailingSpace) noexcept;
}
