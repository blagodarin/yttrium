#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/translation.h>
#include "test_utils.h"

#include <boost/test/unit_test.hpp>

using Yttrium::Buffer;
using Yttrium::Source;
using Yttrium::TemporaryFile;
using Yttrium::Translation;

BOOST_AUTO_TEST_CASE(translation_load)
{
	const auto t = Translation::load(*Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	BOOST_CHECK_EQUAL(t->translate("hello"), "Hello");
	BOOST_CHECK_EQUAL(t->translate("world"), "world!");
}

BOOST_AUTO_TEST_CASE(translation_load_empty)
{
	const auto t = Translation::load(*Source::from(Buffer{}));
	BOOST_CHECK_EQUAL(t->translate("hello"), "hello");
	BOOST_CHECK_EQUAL(t->translate("world"), "world");
}

BOOST_AUTO_TEST_CASE(translation_remove_obsolete)
{
	const auto t = Translation::load(*Source::from(::make_buffer("tr \"hello\" \"Hello\" tr \"world\" \"world!\"")));
	t->add("world");
	t->remove_obsolete();

	TemporaryFile f;
	BOOST_REQUIRE(t->save(f.name()));
	BOOST_CHECK_EQUAL(Source::from(f.name())->to_string(), "tr \"world\" \"world!\"\n");
}

BOOST_AUTO_TEST_CASE(translation_save)
{
	const auto t = Translation::load(*Source::from(Buffer{}));
	t->add("hello");
	t->add("world");

	TemporaryFile f;
	BOOST_REQUIRE(t->save(f.name()));
	BOOST_CHECK_EQUAL(Source::from(f.name())->to_string(), "tr \"hello\" \"\"\ntr \"world\" \"\"\n");
}

BOOST_AUTO_TEST_CASE(translation_save_empty)
{
	const auto t = Translation::load(*Source::from(Buffer{}));

	TemporaryFile f;
	BOOST_REQUIRE(t->save(f.name()));
	BOOST_CHECK_EQUAL(Source::from(f.name())->to_string(), "");
}

BOOST_AUTO_TEST_CASE(translation_save_invalid)
{
	const auto t = Translation::load(*Source::from(Buffer{}));
	BOOST_CHECK(!t->save("/inv*lid"));
}
