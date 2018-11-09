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
		Buffer subbuffer(source->size());
		CHECK(subsource->read_at(0, subbuffer.data(), subbuffer.size()) == subsource->size());
		CHECK(!std::memcmp(subbuffer.begin(), buffer.begin(), subsource->size()));
	}
	{
		const auto subsource = Source::from(source, 0, source->size() / 2);
		REQUIRE(subsource->size() == source->size() / 2);
		Buffer subbuffer(source->size());
		CHECK(subsource->read_at(0, subbuffer.data(), subbuffer.size()) == subsource->size());
		CHECK(!std::memcmp(subbuffer.begin(), buffer.begin(), subsource->size()));
	}
	{
		const auto subsource = Source::from(source, source->size() / 2, source->size() - source->size() / 2);
		REQUIRE(subsource->size() == source->size() - source->size() / 2);
		Buffer subbuffer(source->size());
		CHECK(subsource->read_at(0, subbuffer.data(), subbuffer.size()) == subsource->size());
		CHECK(!std::memcmp(subbuffer.begin(), buffer.begin() + source->size() / 2, subsource->size()));
	}
}

TEST_CASE("source.name")
{
	Yttrium::TemporaryFile tf;
	const auto s = Source::from(tf.name());
	REQUIRE(s);
	CHECK(s->name() == tf.name());
}

TEST_CASE("source.property")
{
	const auto source = Source::from(nullptr, 0);
	REQUIRE(source);
	CHECK(source->property("name") == "");
	source->set_property("name", "value");
	CHECK(source->property("name") == "value");
	source->set_property("name", "another value");
	CHECK(source->property("name") == "another value");
}

TEST_CASE("source.size")
{
	CHECK(Source::from(Buffer{})->size() == 0);
	CHECK(Source::from(Buffer{1})->size() == 1);
	CHECK(Source::from(Buffer{997})->size() == 997);
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
