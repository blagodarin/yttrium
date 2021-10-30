// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary.h>
#include <yttrium/storage/writer.h>

#include <doctest/doctest.h>

TEST_CASE("temporary_file")
{
	Yt::TemporaryFile file;
	{
		const auto source = Yt::Source::from(file);
		REQUIRE(source);
		CHECK(source->size() == 0);
	}
	{
		Yt::Writer writer{ file };
		CHECK(writer.size() == 0);
		writer.write(int64_t{ -1 });
		CHECK(writer.size() == sizeof(int64_t));
	}
	{
		const auto source = Yt::Source::from(file);
		REQUIRE(source);
		CHECK(source->size() == sizeof(int64_t));
		Yt::Reader reader{ *source };
		int64_t data = 0;
		CHECK(reader.read(data));
		CHECK(data == -1);
	}
	{
		Yt::Writer writer{ file };
		CHECK(writer.size() == 0);
	}
	{
		const auto source = Yt::Source::from(file);
		REQUIRE(source);
		CHECK(source->size() == 0);
	}
}
