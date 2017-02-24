#include <yttrium/storage/reader.h>
#include <yttrium/string.h>
#include "iostream.h"

#include <cstdlib>
#include <cstring>

#include <boost/test/unit_test.hpp>

namespace Yttrium
{
	inline Buffer make_random_buffer(size_t size)
	{
		Buffer buffer(size);
		for (auto& byte : buffer)
			byte = rand() % std::numeric_limits<uint8_t>::max();
		return buffer;
	}
}

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_reader_construction)
{
	BOOST_CHECK(!Reader());
	BOOST_CHECK(!Reader("no_such_file"));
	BOOST_CHECK(Reader(nullptr, 0));
	BOOST_CHECK(Reader(Buffer()));
}

BOOST_AUTO_TEST_CASE(test_reader_size)
{
	BOOST_CHECK_EQUAL(Reader().size(), 0);
	BOOST_CHECK_EQUAL(Reader(Buffer()).size(), 0);
	BOOST_CHECK_EQUAL(Reader(Buffer(1)).size(), 1);
	BOOST_CHECK_EQUAL(Reader(Buffer(997)).size(), 997);
}

BOOST_AUTO_TEST_CASE(test_reader_offset)
{
	BOOST_CHECK_EQUAL(Reader().offset(), 0);
	BOOST_CHECK_EQUAL(Reader(Buffer()).offset(), 0);
	BOOST_CHECK_EQUAL(Reader(Buffer(1)).offset(), 0);
	BOOST_CHECK_EQUAL(Reader(Buffer(997)).offset(), 0);
}

BOOST_AUTO_TEST_CASE(test_reader_property)
{
	Reader reader(nullptr, 0);
	BOOST_REQUIRE(reader);
	BOOST_CHECK_EQUAL(reader.property("name"), "");
	reader.set_property("name", "value");
	BOOST_CHECK_EQUAL(reader.property("name"), "value");
	reader.set_property("name", "another value");
	BOOST_CHECK_EQUAL(reader.property("name"), "another value");
}

BOOST_AUTO_TEST_CASE(test_reader_seek)
{
	Reader reader(Buffer(997));
	BOOST_CHECK(reader.seek(1));
	BOOST_CHECK_EQUAL(reader.offset(), 1);
	BOOST_CHECK(reader.seek(499));
	BOOST_CHECK_EQUAL(reader.offset(), 499);
	BOOST_CHECK(!reader.seek(1000));
	BOOST_CHECK_EQUAL(reader.offset(), 499);
	BOOST_CHECK(reader.seek(997));
	BOOST_CHECK_EQUAL(reader.offset(), 997);
	BOOST_CHECK(!reader.seek(998));
}

BOOST_AUTO_TEST_CASE(test_reader_skip)
{
	Reader reader(Buffer(997));
	BOOST_CHECK(reader.skip(1));
	BOOST_CHECK_EQUAL(reader.offset(), 1);
	BOOST_CHECK(reader.skip(499));
	BOOST_CHECK_EQUAL(reader.offset(), 500);
	BOOST_CHECK(!reader.skip(1000));
	BOOST_CHECK_EQUAL(reader.offset(), 500);
	BOOST_CHECK(reader.skip(497));
	BOOST_CHECK_EQUAL(reader.offset(), 997);
	BOOST_CHECK(!reader.skip(1));
}

BOOST_AUTO_TEST_CASE(test_reader_read_all_buffer)
{
	const auto expected = ::make_random_buffer(997);
	Buffer actual;
	BOOST_REQUIRE(Reader(expected.data(), expected.size()).read_all(actual));
	BOOST_CHECK_EQUAL(expected, actual);
	BOOST_REQUIRE(actual.capacity() > actual.size());
	BOOST_CHECK_EQUAL(actual[actual.size()], '\0');
}

BOOST_AUTO_TEST_CASE(test_reader_read_all_string)
{
	const auto expected = ::make_random_buffer(997);
	std::string actual;
	BOOST_REQUIRE(Reader(expected.data(), expected.size()).read_all(actual));
	BOOST_CHECK_EQUAL(actual.size(), expected.size());
	BOOST_CHECK(!::memcmp(actual.data(), expected.data(), expected.size()));
}

BOOST_AUTO_TEST_CASE(test_reader_read_line_std_string)
{
	{
		const std::string data = "hello\nworld";
		Reader reader(data.data(), data.size());
		std::string line;
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "hello");
		BOOST_REQUIRE_EQUAL(reader.offset(), 6);
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "world");
		BOOST_CHECK_EQUAL(reader.offset(), 11);
		BOOST_CHECK(!reader.read_line(line));
		BOOST_CHECK(line.empty());
	}
	{
		const std::string data = "hello\rworld";
		Reader reader(data.data(), data.size());
		std::string line;
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "hello");
		BOOST_REQUIRE_EQUAL(reader.offset(), 6);
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "world");
		BOOST_CHECK_EQUAL(reader.offset(), 11);
		BOOST_CHECK(!reader.read_line(line));
		BOOST_CHECK(line.empty());
	}
	{
		const std::string data = "hello\r\nworld";
		Reader reader(data.data(), data.size());
		std::string line;
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "hello");
		BOOST_REQUIRE_EQUAL(reader.offset(), 7);
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "world");
		BOOST_CHECK_EQUAL(reader.offset(), 12);
		BOOST_CHECK(!reader.read_line(line));
		BOOST_CHECK(line.empty());
	}
	{
		const std::string data = "hello\n\rworld";
		Reader reader(data.data(), data.size());
		std::string line;
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "hello");
		BOOST_REQUIRE_EQUAL(reader.offset(), 6);
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK(line.empty());
		BOOST_REQUIRE_EQUAL(reader.offset(), 7);
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "world");
		BOOST_CHECK_EQUAL(reader.offset(), 12);
		BOOST_CHECK(!reader.read_line(line));
		BOOST_CHECK(line.empty());
	}
	{
		const std::string data = "hello";
		Reader reader(data.data(), data.size());
		std::string line;
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "hello");
		BOOST_REQUIRE_EQUAL(reader.offset(), 5);
		BOOST_REQUIRE(!reader.read_line(line));
		BOOST_CHECK(line.empty());
	}
	{
		const std::string data = "hello\n";
		Reader reader(data.data(), data.size());
		std::string line;
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "hello");
		BOOST_REQUIRE_EQUAL(reader.offset(), 6);
		BOOST_REQUIRE(!reader.read_line(line));
		BOOST_CHECK(line.empty());
	}
	{
		const std::string data = "hello\r";
		Reader reader(data.data(), data.size());
		std::string line;
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "hello");
		BOOST_REQUIRE_EQUAL(reader.offset(), 6);
		BOOST_REQUIRE(!reader.read_line(line));
		BOOST_CHECK(line.empty());
	}
	{
		const std::string data = "hello\r\n";
		Reader reader(data.data(), data.size());
		std::string line;
		BOOST_REQUIRE(reader.read_line(line));
		BOOST_CHECK_EQUAL(line, "hello");
		BOOST_REQUIRE_EQUAL(reader.offset(), 7);
		BOOST_REQUIRE(!reader.read_line(line));
		BOOST_CHECK(line.empty());
	}
}

BOOST_AUTO_TEST_CASE(test_reader_to_buffer)
{
	const auto expected = ::make_random_buffer(997);
	const auto actual = Reader(expected.data(), expected.size()).to_buffer();
	BOOST_CHECK_EQUAL(expected, actual);
	BOOST_REQUIRE(actual.capacity() > actual.size());
	BOOST_CHECK_EQUAL(actual[actual.size()], '\0');
}

BOOST_AUTO_TEST_CASE(test_reader_to_string)
{
	const auto expected = ::make_random_buffer(997);
	const auto actual = Reader(expected.data(), expected.size()).to_string();
	BOOST_CHECK_EQUAL(actual.size(), expected.size());
	BOOST_CHECK(!::memcmp(actual.data(), expected.data(), expected.size()));
}

BOOST_AUTO_TEST_CASE(test_reader_reader)
{
	const auto source = ::make_random_buffer(997);
	const Reader reader(source.data(), source.size());
	{
		Reader subreader(reader, 0, reader.size());
		BOOST_CHECK_EQUAL(subreader.offset(), 0);
		BOOST_REQUIRE_EQUAL(subreader.size(), reader.size());
		Buffer buffer(reader.size());
		BOOST_CHECK_EQUAL(subreader.read(buffer.data(), reader.size()), subreader.size());
		BOOST_CHECK(!::memcmp(buffer.begin(), source.begin(), subreader.size()));
	}
	{
		Reader subreader(reader, 0, reader.size() / 2);
		BOOST_CHECK_EQUAL(subreader.offset(), 0);
		BOOST_REQUIRE_EQUAL(subreader.size(), reader.size() / 2);
		Buffer buffer(reader.size());
		BOOST_CHECK_EQUAL(subreader.read(buffer.data(), reader.size()), subreader.size());
		BOOST_CHECK(!::memcmp(buffer.begin(), source.begin(), subreader.size()));
	}
	{
		Reader subreader(reader, reader.size() / 2, reader.size() - reader.size() / 2);
		BOOST_CHECK_EQUAL(subreader.offset(), 0);
		BOOST_REQUIRE_EQUAL(subreader.size(), reader.size() - reader.size() / 2);
		Buffer buffer(reader.size());
		BOOST_CHECK_EQUAL(subreader.read(buffer.data(), reader.size()), subreader.size());
		BOOST_CHECK(!::memcmp(buffer.begin(), source.begin() + reader.size() / 2, subreader.size()));
	}
}
