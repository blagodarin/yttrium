#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include <yttrium/string.h>
#include "iostream.h"

#include <boost/test/unit_test.hpp>

using Yttrium::Buffer;
using Yttrium::Reader;
using Yttrium::Source;

BOOST_AUTO_TEST_CASE(reader_size)
{
	{
		const auto source = Source::from(Buffer{});
		BOOST_CHECK_EQUAL(Reader{*source}.size(), 0);
	}
	{
		const auto source = Source::from(Buffer{1});
		BOOST_CHECK_EQUAL(Reader{*source}.size(), 1);
	}
	{
		const auto source = Source::from(Buffer{997});
		BOOST_CHECK_EQUAL(Reader{*source}.size(), 997);
	}
}

BOOST_AUTO_TEST_CASE(reader_offset)
{
	{
		const auto source = Source::from(Buffer{});
		BOOST_CHECK_EQUAL(Reader{*source}.offset(), 0);
	}
	{
		const auto source = Source::from(Buffer{1});
		BOOST_CHECK_EQUAL(Reader{*source}.offset(), 0);
	}
	{
		const auto source = Source::from(Buffer{997});
		BOOST_CHECK_EQUAL(Reader{*source}.offset(), 0);
	}
}

BOOST_AUTO_TEST_CASE(reader_seek)
{
	const auto source = Source::from(Buffer{997});
	Reader reader{*source};
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

BOOST_AUTO_TEST_CASE(reader_skip)
{
	const auto source = Source::from(Buffer{997});
	Reader reader{*source};
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

BOOST_AUTO_TEST_CASE(reader_read_line)
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
				const auto source = Source::from(data.data(), data.size());
				BOOST_REQUIRE(source->data());
				Reader reader{*source};
				std::string line;
				BOOST_REQUIRE(reader.read_line(line));
				BOOST_CHECK_EQUAL(line, text);
				BOOST_REQUIRE_EQUAL(reader.offset(), data.size());
				BOOST_REQUIRE(!reader.read_line(line));
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
				const auto source = Source::from(Source::from(data.data(), data.size()), 0, data.size());
				BOOST_REQUIRE(!source->data());
				Reader reader{*source};
				std::string line;
				BOOST_REQUIRE(reader.read_line(line));
				BOOST_CHECK_EQUAL(line, text);
				BOOST_REQUIRE_EQUAL(reader.offset(), data.size());
				BOOST_REQUIRE(!reader.read_line(line));
				BOOST_CHECK(line.empty());
			}
		}
	}
}
