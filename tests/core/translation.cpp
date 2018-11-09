#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/translation.h>
#include "../common/utils.h"

#include <catch2/catch.hpp>

using Yttrium::Buffer;
using Yttrium::Source;
using Yttrium::TemporaryFile;
using Yttrium::Translation;

TEST_CASE("translation.load")
{
	const auto t = Translation::load(*Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	CHECK(t->translate("hello") == "Hello");
	CHECK(t->translate("world") == "world!");
}

TEST_CASE("translation.load.empty")
{
	const auto t = Translation::load(*Source::from(Buffer{}));
	CHECK(t->translate("hello") == "hello");
	CHECK(t->translate("world") == "world");
}

TEST_CASE("translation.remove_obsolete")
{
	const auto t = Translation::load(*Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	t->add("world");
	t->remove_obsolete();

	TemporaryFile f;
	REQUIRE(t->save(f.name()));
	CHECK(Source::from(f.name())->to_string() == "tr \"world\" \"world!\"\n");
}

TEST_CASE("translation.save")
{
	const auto t = Translation::load(*Source::from(Buffer{}));
	t->add("hello");
	t->add("world");

	TemporaryFile f;
	REQUIRE(t->save(f.name()));
	CHECK(Source::from(f.name())->to_string() == "tr \"hello\" \"\"\ntr \"world\" \"\"\n");
}

TEST_CASE("translation.save.empty")
{
	const auto t = Translation::load(*Source::from(Buffer{}));

	TemporaryFile f;
	REQUIRE(t->save(f.name()));
	CHECK(Source::from(f.name())->to_string().empty());
}

TEST_CASE("translation.save.invalid")
{
	const auto t = Translation::load(*Source::from(Buffer{}));
	CHECK(!t->save("/inv*lid"));
}
