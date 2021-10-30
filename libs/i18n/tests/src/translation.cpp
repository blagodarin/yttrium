// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/buffer.h>
#include <yttrium/i18n/translation.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary.h>
#include <yttrium/storage/writer.h>

#include <doctest/doctest.h>

namespace
{
	Yt::Buffer make_buffer(std::string_view text)
	{
		return { text.size(), text.data() };
	}
}

TEST_CASE("translation.load")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	CHECK(translation->translate("hello") == "Hello");
	CHECK(translation->translate("world") == "world!");
}

TEST_CASE("translation.load.empty")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(Yt::Buffer{}));
	CHECK(translation->translate("hello") == "hello");
	CHECK(translation->translate("world") == "world");
}

TEST_CASE("translation.remove_obsolete")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	translation->add("world");
	translation->remove_obsolete();

	Yt::TemporaryFile file;
	translation->save(Yt::Writer{ file });
	CHECK(Yt::Source::from(file)->to_string() == "tr \"world\" \"world!\"\n");
}

TEST_CASE("translation.save")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(Yt::Buffer{}));
	translation->add("hello");
	translation->add("world");

	Yt::TemporaryFile file;
	translation->save(Yt::Writer{ file });
	CHECK(Yt::Source::from(file)->to_string() == "tr \"hello\" \"\"\ntr \"world\" \"\"\n");
}

TEST_CASE("translation.save.empty")
{
	const auto translation = Yt::Translation::load(*Yt::Source::from(Yt::Buffer{}));

	Yt::TemporaryFile file;
	translation->save(Yt::Writer{ file });
	CHECK(Yt::Source::from(file)->to_string().empty());
}
