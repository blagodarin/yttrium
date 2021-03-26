// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary.h>
#include <yttrium/test/utils.h>

#include <cstring>

#include <doctest.h>

TEST_CASE("source.from")
{
	CHECK(!Yt::Source::from("no_such_file"));
	CHECK(Yt::Source::from(nullptr, 0));
	CHECK(Yt::Source::from(Yt::Buffer{}));
}

TEST_CASE("source.from.source")
{
	const auto buffer = ::make_random_buffer(997);
	const auto source = std::shared_ptr<Yt::Source>{ Yt::Source::from(buffer.data(), buffer.size()) };
	{
		const auto subsource = Yt::Source::from(source, 0, source->size());
		REQUIRE(subsource->size() == source->size());
		Yt::Buffer subbuffer{ static_cast<size_t>(source->size()) };
		CHECK(subsource->read_at(0, subbuffer.data(), subbuffer.size()) == subsource->size());
		CHECK(!std::memcmp(subbuffer.begin(), buffer.begin(), static_cast<size_t>(subsource->size())));
	}
	{
		const auto subsource = Yt::Source::from(source, 0, source->size() / 2);
		REQUIRE(subsource->size() == source->size() / 2);
		Yt::Buffer subbuffer{ static_cast<size_t>(source->size()) };
		CHECK(subsource->read_at(0, subbuffer.data(), subbuffer.size()) == subsource->size());
		CHECK(!std::memcmp(subbuffer.begin(), buffer.begin(), static_cast<size_t>(subsource->size())));
	}
	{
		const auto subsource = Yt::Source::from(source, source->size() / 2, source->size() - source->size() / 2);
		REQUIRE(subsource->size() == source->size() - source->size() / 2);
		Yt::Buffer subbuffer{ static_cast<size_t>(source->size()) };
		CHECK(subsource->read_at(0, subbuffer.data(), subbuffer.size()) == subsource->size());
		CHECK(!std::memcmp(subbuffer.begin(), buffer.begin() + source->size() / 2, static_cast<size_t>(subsource->size())));
	}
}

TEST_CASE("source.size")
{
	CHECK(Yt::Source::from(Yt::Buffer{})->size() == 0);
	CHECK(Yt::Source::from(Yt::Buffer{ 1 })->size() == 1);
	CHECK(Yt::Source::from(Yt::Buffer{ 997 })->size() == 997);
}

TEST_CASE("source.to_buffer")
{
	const auto expected = ::make_random_buffer(997);
	const auto actual = Yt::Source::from(expected.data(), expected.size())->to_buffer();
	CHECK(expected == actual);
	REQUIRE(actual.capacity() > actual.size());
	CHECK(actual[actual.size()] == '\0');
}

TEST_CASE("source.to_string")
{
	const auto expected = ::make_random_buffer(997);
	const auto actual = Yt::Source::from(expected.data(), expected.size())->to_string();
	CHECK(actual.size() == expected.size());
	CHECK(!std::memcmp(actual.data(), expected.data(), expected.size()));
}
