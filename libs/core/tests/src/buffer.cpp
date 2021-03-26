// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/memory/buffer.h>

#include <doctest.h>

using Yt::Buffer;

const auto granularity = Buffer::memory_granularity();

TEST_CASE("buffer")
{
	{
		Buffer buffer;
		CHECK(buffer.size() == 0);
		CHECK(buffer.capacity() == 0);
	}
	{
		Buffer buffer{ 0 };
		CHECK(buffer.size() == 0);
		CHECK(buffer.capacity() == 0);
	}
	{
		Buffer buffer{ 1 };
		CHECK(buffer.size() == 1);
		CHECK(buffer.capacity() == granularity);
	}
	{
		Buffer buffer{ granularity };
		CHECK(buffer.size() == granularity);
		CHECK(buffer.capacity() == granularity);
	}
	{
		Buffer buffer{ granularity + 1 };
		CHECK(buffer.size() == granularity + 1);
		CHECK(buffer.capacity() == granularity * 2);
	}
}

TEST_CASE("buffer.try_reserve")
{
	Buffer buffer;

	REQUIRE(buffer.try_reserve(1));
	CHECK(buffer.size() == 0);
	CHECK(buffer.capacity() == granularity);

	{
		const auto data = buffer.data();
		CHECK(data);
		REQUIRE(buffer.try_reserve(granularity));
		CHECK(buffer.size() == 0);
		CHECK(buffer.capacity() == granularity);
		CHECK(buffer.data() == data);
	}

	REQUIRE(buffer.try_reserve(granularity + 1));
	CHECK(buffer.size() == 0);
	CHECK(buffer.capacity() == granularity * 2);

	{
		const auto data = buffer.data();
		CHECK(data);
		REQUIRE(buffer.try_reserve(granularity));
		CHECK(buffer.size() == 0);
		CHECK(buffer.capacity() == granularity * 2);
		CHECK(buffer.data() == data);
	}
}

TEST_CASE("buffer.try_reset")
{
	Buffer buffer;

	REQUIRE(buffer.try_reset(1));
	CHECK(buffer.size() == 1);
	CHECK(buffer.capacity() == granularity);

	REQUIRE(buffer.try_reset(granularity + 1));
	CHECK(buffer.size() == granularity + 1);
	CHECK(buffer.capacity() == granularity * 2);
}

TEST_CASE("buffer.try_resize")
{
	Buffer buffer;

	REQUIRE(buffer.try_resize(1));
	CHECK(buffer.size() == 1);
	CHECK(buffer.capacity() == granularity);

	REQUIRE(buffer.try_resize(granularity + 1));
	CHECK(buffer.size() == granularity + 1);
	CHECK(buffer.capacity() == granularity * 2);

	{
		const auto data = buffer.data();
		CHECK(data);
		REQUIRE(buffer.try_resize(granularity));
		CHECK(buffer.size() == granularity);
		CHECK(buffer.capacity() == granularity * 2);
		CHECK(buffer.data() == data);
	}
}
