#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include "iostream.h"
#include "test_utils.h"

#include <boost/test/unit_test.hpp>

using Yttrium::Buffer;
using Yttrium::Source;

BOOST_AUTO_TEST_CASE(source_from)
{
	BOOST_CHECK(!Source::from("no_such_file"));
	BOOST_CHECK(Source::from(nullptr, 0));
	BOOST_CHECK(Source::from(Buffer{}));
}

BOOST_AUTO_TEST_CASE(source_from_source)
{
	const auto buffer = ::make_random_buffer(997);
	const std::shared_ptr<Source> source = Source::from(buffer.data(), buffer.size());
	{
		const auto subsource = Source::from(source, 0, source->size());
		BOOST_REQUIRE_EQUAL(subsource->size(), source->size());
		Buffer subbuffer(source->size());
		BOOST_CHECK_EQUAL(subsource->read_at(0, subbuffer.data(), subbuffer.size()), subsource->size());
		BOOST_CHECK(!::memcmp(subbuffer.begin(), buffer.begin(), subsource->size()));
	}
	{
		const auto subsource = Source::from(source, 0, source->size() / 2);
		BOOST_REQUIRE_EQUAL(subsource->size(), source->size() / 2);
		Buffer subbuffer(source->size());
		BOOST_CHECK_EQUAL(subsource->read_at(0, subbuffer.data(), subbuffer.size()), subsource->size());
		BOOST_CHECK(!::memcmp(subbuffer.begin(), buffer.begin(), subsource->size()));
	}
	{
		const auto subsource = Source::from(source, source->size() / 2, source->size() - source->size() / 2);
		BOOST_REQUIRE_EQUAL(subsource->size(), source->size() - source->size() / 2);
		Buffer subbuffer(source->size());
		BOOST_CHECK_EQUAL(subsource->read_at(0, subbuffer.data(), subbuffer.size()), subsource->size());
		BOOST_CHECK(!::memcmp(subbuffer.begin(), buffer.begin() + source->size() / 2, subsource->size()));
	}
}

BOOST_AUTO_TEST_CASE(source_name)
{
	Yttrium::TemporaryFile tf;
	const auto s = Source::from(tf.name());
	BOOST_REQUIRE(s);
	BOOST_CHECK_EQUAL(s->name(), tf.name());
}

BOOST_AUTO_TEST_CASE(source_properties)
{
	const auto source = Source::from(nullptr, 0);
	BOOST_REQUIRE(source);
	BOOST_CHECK_EQUAL(source->property("name"), "");
	source->set_property("name", "value");
	BOOST_CHECK_EQUAL(source->property("name"), "value");
	source->set_property("name", "another value");
	BOOST_CHECK_EQUAL(source->property("name"), "another value");
}

BOOST_AUTO_TEST_CASE(source_size)
{
	BOOST_CHECK_EQUAL(Source::from(Buffer{})->size(), 0);
	BOOST_CHECK_EQUAL(Source::from(Buffer{1})->size(), 1);
	BOOST_CHECK_EQUAL(Source::from(Buffer{997})->size(), 997);
}

BOOST_AUTO_TEST_CASE(source_to_buffer)
{
	const auto expected = ::make_random_buffer(997);
	const auto actual = Source::from(expected.data(), expected.size())->to_buffer();
	BOOST_CHECK_EQUAL(expected, actual);
	BOOST_REQUIRE(actual.capacity() > actual.size());
	BOOST_CHECK_EQUAL(actual[actual.size()], '\0');
}

BOOST_AUTO_TEST_CASE(source_to_string)
{
	const auto expected = ::make_random_buffer(997);
	const auto actual = Source::from(expected.data(), expected.size())->to_string();
	BOOST_CHECK_EQUAL(actual.size(), expected.size());
	BOOST_CHECK(!std::memcmp(actual.data(), expected.data(), expected.size()));
}
