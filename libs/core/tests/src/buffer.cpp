//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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
