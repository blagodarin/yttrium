#include <yttrium/dir.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(dir_test)
{
	DECLARE_MEMORY_MANAGER;

	Dir dir;

	BOOST_CHECK(!dir.is_opened());

	BOOST_REQUIRE(dir.open("tests/dir"));

	BOOST_CHECK(dir.is_opened());

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
