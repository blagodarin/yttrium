//
// Copyright 2019 Sergei Blagodarin
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
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include "../common/utils.h"
#include "iostream.h"

#include <cstring>

#include <catch2/catch.hpp>

using Yttrium::Buffer;
using Yttrium::Source;

TEST_CASE("source.from")
{
	CHECK(!Source::from("no_such_file"));
	CHECK(Source::from(nullptr, 0));
	CHECK(Source::from(Buffer{}));
}

TEST_CASE("source.from.source")
{
	const auto buffer = ::make_random_buffer(997);
	const std::shared_ptr<Source> source = Source::from(buffer.data(), buffer.size());
	{
		const auto subsource = Source::from(source, 0, source->size());
		REQUIRE(subsource->size() == source->size());
		Buffer subbuffer{ static_cast<size_t>(source->size()) };
		CHECK(subsource->read_at(0, subbuffer.data(), subbuffer.size()) == subsource->size());
		CHECK(!std::memcmp(subbuffer.begin(), buffer.begin(), static_cast<size_t>(subsource->size())));
	}
	{
		const auto subsource = Source::from(source, 0, source->size() / 2);
		REQUIRE(subsource->size() == source->size() / 2);
		Buffer subbuffer{ static_cast<size_t>(source->size()) };
		CHECK(subsource->read_at(0, subbuffer.data(), subbuffer.size()) == subsource->size());
		CHECK(!std::memcmp(subbuffer.begin(), buffer.begin(), static_cast<size_t>(subsource->size())));
	}
	{
		const auto subsource = Source::from(source, source->size() / 2, source->size() - source->size() / 2);
		REQUIRE(subsource->size() == source->size() - source->size() / 2);
		Buffer subbuffer{ static_cast<size_t>(source->size()) };
		CHECK(subsource->read_at(0, subbuffer.data(), subbuffer.size()) == subsource->size());
		CHECK(!std::memcmp(subbuffer.begin(), buffer.begin() + source->size() / 2, static_cast<size_t>(subsource->size())));
	}
}

TEST_CASE("source.name")
{
	Yttrium::TemporaryFile tf;
	const auto s = Source::from(tf.name());
	REQUIRE(s);
	CHECK(s->name() == tf.name());
}

TEST_CASE("source.size")
{
	CHECK(Source::from(Buffer{})->size() == 0);
	CHECK(Source::from(Buffer{ 1 })->size() == 1);
	CHECK(Source::from(Buffer{ 997 })->size() == 997);
}

TEST_CASE("source.to_buffer")
{
	const auto expected = ::make_random_buffer(997);
	const auto actual = Source::from(expected.data(), expected.size())->to_buffer();
	CHECK(expected == actual);
	REQUIRE(actual.capacity() > actual.size());
	CHECK(actual[actual.size()] == '\0');
}

TEST_CASE("source.to_string")
{
	const auto expected = ::make_random_buffer(997);
	const auto actual = Source::from(expected.data(), expected.size())->to_string();
	CHECK(actual.size() == expected.size());
	CHECK(!std::memcmp(actual.data(), expected.data(), expected.size()));
}
