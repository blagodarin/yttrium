// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "../../src/ion/utils.h"

#include <cstring>
#include <memory>

#include <doctest.h>

TEST_CASE("utils_string.time_from_chars")
{
	// Helps sanitizer to catch out-of-bounds errors.
	const auto time_from_chars = [](std::string_view text) {
		const auto copy = std::make_unique<char[]>(text.size());
		std::memcpy(copy.get(), text.data(), text.size());
		return Yt::time_from_chars({ copy.get(), text.size() });
	};

	CHECK(time_from_chars("1") == 1'000);
	CHECK(time_from_chars("1:2") == 62'000);
	CHECK(time_from_chars("1:1:2") == 3662'000);
	CHECK(time_from_chars("1:1:2.5") == 3662'500);
	CHECK(time_from_chars("-10:1:2.25") == -36062'250);

	CHECK(time_from_chars("-01:23:45") == -5025'000);
	CHECK(time_from_chars("+01:23:45.") == 5025'000);
	CHECK(time_from_chars("-01:23:45.6") == -5025'600);
	CHECK(time_from_chars("+01:23:45.67") == 5025'670);
	CHECK(time_from_chars("-01:23:45.678") == -5025'678);
	CHECK(time_from_chars("+01:23:45.6789") == 5025'678);

	CHECK(time_from_chars("") == 0);
	CHECK(time_from_chars(".") == 0);
	CHECK(time_from_chars("+::.") == 0);
	CHECK(time_from_chars("+::.1") == 100);
	CHECK(time_from_chars("-1.1") == -1'100);
	CHECK(time_from_chars("-1:.1") == -60'100);
	CHECK(time_from_chars("-1::.1") == -3600'100);
	CHECK(time_from_chars("-1:::.1") == -3600'000);
}
