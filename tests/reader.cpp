#include <yttrium/io/reader.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/string.h>

#include <cstdlib>
#include <cstring>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Yttrium
{
	inline Buffer make_random_buffer(size_t size)
	{
		Buffer buffer(size);
		for (auto& byte : buffer)
			byte = rand() % UINT8_MAX;
		return buffer;
	}

	inline std::ostream& operator<<(std::ostream& stream, const Buffer& buffer)
	{
		return stream << "Buffer(" << buffer.size() << ")";
	}
}

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_reader_read_all_buffer)
{
	const auto expected = std::make_shared<const Buffer>(::make_random_buffer(100003));

	Buffer actual;
	BOOST_REQUIRE(Reader(expected).read_all(actual));

	BOOST_CHECK_EQUAL(*expected, actual);
	BOOST_REQUIRE(actual.capacity() > actual.size());
	BOOST_CHECK_EQUAL(actual[actual.size()], '\0');
}

BOOST_AUTO_TEST_CASE(test_reader_read_all_string)
{
	const auto expected = std::make_shared<const Buffer>(::make_random_buffer(100003));

	String actual;
	BOOST_REQUIRE(Reader(expected).read_all(actual));

	BOOST_CHECK_EQUAL(actual.size(), expected->size());
	BOOST_CHECK(!::memcmp(actual.text(), expected->data(), expected->size()));
}

BOOST_AUTO_TEST_CASE(test_reader_to_buffer)
{
	const auto expected = std::make_shared<const Buffer>(::make_random_buffer(100003));

	const auto actual = Reader(expected).to_buffer();

	BOOST_CHECK_EQUAL(*expected, actual);
	BOOST_REQUIRE(actual.capacity() > actual.size());
	BOOST_CHECK_EQUAL(actual[actual.size()], '\0');
}

BOOST_AUTO_TEST_CASE(test_reader_to_string)
{
	const auto expected = std::make_shared<const Buffer>(::make_random_buffer(100003));

	const auto actual = Reader(expected).to_string();

	BOOST_CHECK_EQUAL(actual.size(), expected->size());
	BOOST_CHECK(!::memcmp(actual.text(), expected->data(), expected->size()));
}
