// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/buffer_appender.h>
#include <yttrium/base/numeric.h>

#include <cstring>

#include <doctest.h>

TEST_CASE("buffer_appender")
{
	Yt::Buffer b;
	Yt::BufferAppender<uint32_t> ba{ b };
	CHECK(ba.count() == 0);
	ba << Yt::make_cc('1', '2', '3', '4');
	ba << Yt::make_cc('5', '6', '7', '8');
	CHECK(ba.count() == 2);
	REQUIRE(b.size() == 8);
	CHECK(!std::memcmp("12345678", b.data(), b.size()));
}
