// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>

#include <memory>

#include <doctest.h>

TEST_CASE("reader.size")
{
	{
		const auto source = Yt::Source::from(Yt::Buffer{});
		CHECK(Yt::Reader{ *source }.size() == 0);
	}
	{
		const auto source = Yt::Source::from(Yt::Buffer{ 1 });
		CHECK(Yt::Reader{ *source }.size() == 1);
	}
	{
		const auto source = Yt::Source::from(Yt::Buffer{ 997 });
		CHECK(Yt::Reader{ *source }.size() == 997);
	}
}

TEST_CASE("reader.offset")
{
	{
		const auto source = Yt::Source::from(Yt::Buffer{});
		CHECK(Yt::Reader{ *source }.offset() == 0);
	}
	{
		const auto source = Yt::Source::from(Yt::Buffer{ 1 });
		CHECK(Yt::Reader{ *source }.offset() == 0);
	}
	{
		const auto source = Yt::Source::from(Yt::Buffer{ 997 });
		CHECK(Yt::Reader{ *source }.offset() == 0);
	}
}

TEST_CASE("reader.seek")
{
	const auto source = Yt::Source::from(Yt::Buffer{ 997 });
	Yt::Reader reader{ *source };
	CHECK(reader.seek(1));
	CHECK(reader.offset() == 1);
	CHECK(reader.seek(499));
	CHECK(reader.offset() == 499);
	CHECK(!reader.seek(1000));
	CHECK(reader.offset() == 499);
	CHECK(reader.seek(997));
	CHECK(reader.offset() == 997);
	CHECK(!reader.seek(998));
}

TEST_CASE("reader.skip")
{
	const auto source = Yt::Source::from(Yt::Buffer{ 997 });
	Yt::Reader reader{ *source };
	CHECK(reader.skip(1));
	CHECK(reader.offset() == 1);
	CHECK(reader.skip(499));
	CHECK(reader.offset() == 500);
	CHECK(!reader.skip(1000));
	CHECK(reader.offset() == 500);
	CHECK(reader.skip(497));
	CHECK(reader.offset() == 997);
	CHECK(!reader.skip(1));
}

TEST_CASE("reader.read_line")
{
	struct Entry
	{
		size_t text_size;
		std::string newline;
		std::string comment;
	};

	SUBCASE("unbuffered")
	{
		static const Entry entries[]{
			{ 17, "\n", "Short newline" },
			{ 17, "\r", "Short newline #2" },
			{ 17, "\r\n", "Long newline" },
			{ 17, "", "No newline" },
		};

		for (const auto& entry : entries)
		{
			INFO(entry.comment);
			const std::string text(entry.text_size, 'A');
			const std::string data = text + entry.newline;
			const auto source = Yt::Source::from(data.data(), data.size());
			REQUIRE(source->data());
			Yt::Reader reader{ *source };
			std::string line;
			REQUIRE(reader.read_line(line));
			CHECK(line == text);
			REQUIRE(reader.offset() == data.size());
			REQUIRE(!reader.read_line(line));
			CHECK(line.empty());
		}
	}

	SUBCASE("buffered")
	{
		static const Entry entries[]{
			{ 31, "\n", "Short newline, one buffer" },
			{ 30, "\r\n", "Long newline, one buffer" },
			{ 31, "\r\n", "Split long newline" },
			{ 31, "\r", "Potentially split long newline" },
			{ 32, "\n", "Short newline in the second buffer" },
			{ 33, "\r\n", "Text and newline in the second buffer" },
			{ 31, "", "No newline, less than one buffer" },
			{ 32, "", "No newline, exactly one buffer" },
			{ 33, "", "No newline, more than one buffer" },
		};

		for (const auto& entry : entries)
		{
			INFO(entry.comment);
			const std::string text(entry.text_size, 'A');
			const std::string data = text + entry.newline;
			const auto source = Yt::Source::from(Yt::Source::from(data.data(), data.size()), 0, data.size());
			REQUIRE(!source->data());
			Yt::Reader reader{ *source };
			std::string line;
			REQUIRE(reader.read_line(line));
			CHECK(line == text);
			REQUIRE(reader.offset() == data.size());
			REQUIRE(!reader.read_line(line));
			CHECK(line.empty());
		}
	}
}
