// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/memory/buffer.h>
#include <yttrium/test/utils.h>

#include <cstring>
#include <string>

#include <doctest.h>

TEST_CASE("test_utils.make_buffer")
{
	const std::string string = "test";
	const auto buffer = ::make_buffer(string);
	CHECK(buffer.size() == string.size());
	CHECK(!std::memcmp(buffer.data(), string.data(), string.size()));
}
