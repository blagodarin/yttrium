// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/buffer_appender.h>

#include <cstring>

#include <doctest.h>

TEST_CASE("buffer_appender")
{
	Yt::Buffer b;
	Yt::BufferAppender<uint32_t> ba{ b };
	CHECK(ba.count() == 0);
	ba << 0x01010101;
	ba << 0x02020202;
	CHECK(ba.count() == 2);
	REQUIRE(b.size() == 8);
	CHECK(!std::memcmp("\x01\x01\x01\x01\x02\x02\x02\x02", b.data(), b.size()));
}
