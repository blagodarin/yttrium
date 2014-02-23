#include <yttrium/dir.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(dir_test)
{
	DECLARE_MEMORY_MANAGER;

	BOOST_CHECK(Dir::exists("tests/dir"));
	BOOST_CHECK(!Dir::exists("tests/void"));

	BOOST_CHECK(!Dir("tests/void").exists());

	Dir dir("tests/dir");

	BOOST_REQUIRE(dir.exists());

	size_t count = 0;

	for (const StaticString &entry: dir)
	{
		BOOST_CHECK(entry == "." || entry == ".." || entry == "file");
		++count;
	}

	BOOST_CHECK_EQUAL(count, 3);

	// Reiterability check.

	count = 0;

	for (const StaticString &entry: dir)
	{
		BOOST_CHECK(entry == "." || entry == ".." || entry == "file");
		++count;
	}

	BOOST_CHECK_EQUAL(count, 3);
}
