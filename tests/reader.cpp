#include <yttrium/storage/reader.h>
#include <yttrium/string.h>
#include "iostream.h"

#include <random>

#include <boost/test/unit_test.hpp>

using Yttrium::Buffer;
using Yttrium::Reader;

namespace
{
	Buffer make_random_buffer(size_t size)
	{
		std::default_random_engine engine;
		std::uniform_int_distribution<unsigned> distribution{0, std::numeric_limits<uint8_t>::max()}; // Visual C++ 2017 doesn't allow uint8_t distribution.
		Buffer buffer(size);
		for (auto& byte : buffer)
			byte = static_cast<uint8_t>(distribution(engine));
		return buffer;
	}
}

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

BOOST_AUTO_TEST_CASE(test_reader_read_line_unbuffered)
{
	struct Entry
	{
		size_t text_size;
		std::string newline;
		std::string comment;
	};

	BOOST_TEST_CONTEXT("Unbuffered")
	{
		static const Entry entries[] =
		{
			{ 17, "\n",   "Short newline"    },
			{ 17, "\r",   "Short newline #2" },
			{ 17, "\r\n", "Long newline"     },
			{ 17, "",     "No newline"       },
		};

		for (const auto& entry : entries)
		{
			BOOST_TEST_CONTEXT(entry.comment)
			{
				const std::string text(entry.text_size, 'A');
				const std::string data = text + entry.newline;
				Reader unbuffered_reader(data.data(), data.size());
				std::string line;
				BOOST_REQUIRE(unbuffered_reader.read_line(line));
				BOOST_CHECK_EQUAL(line, text);
				BOOST_REQUIRE_EQUAL(unbuffered_reader.offset(), data.size());
				BOOST_REQUIRE(!unbuffered_reader.read_line(line));
				BOOST_CHECK(line.empty());
			}
		}
	}

	BOOST_TEST_CONTEXT("Buffered")
	{
		static const Entry entries[] =
		{
			{ 31, "\n",   "Short newline, one buffer"             },
			{ 30, "\r\n", "Long newline, one buffer"              },
			{ 31, "\r\n", "Split long newline"                    },
			{ 31, "\r",   "Potentially split long newline"        },
			{ 32, "\n",   "Short newline in the second buffer"    },
			{ 33, "\r\n", "Text and newline in the second buffer" },
			{ 31, "",     "No newline, less than one buffer"      },
			{ 32, "",     "No newline, exactly one buffer"        },
			{ 33, "",     "No newline, more than one buffer"      },
		};

		for (const auto& entry : entries)
		{
			BOOST_TEST_CONTEXT(entry.comment)
			{
				const std::string text(entry.text_size, 'A');
				const std::string data = text + entry.newline;
				Reader unbuffered_reader(data.data(), data.size());
				Reader buffered_reader(unbuffered_reader, 0, unbuffered_reader.size());
				std::string line;
				BOOST_REQUIRE(buffered_reader.read_line(line));
				BOOST_CHECK_EQUAL(line, text);
				BOOST_REQUIRE_EQUAL(buffered_reader.offset(), data.size());
				BOOST_REQUIRE(!buffered_reader.read_line(line));
				BOOST_CHECK(line.empty());
			}
		}
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
