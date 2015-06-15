#include <yttrium/dir.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_dir)
{
	DECLARE_MEMORY_MANAGER;

	BOOST_CHECK(Dir::exists("tests/dir"));
	BOOST_CHECK(!Dir::exists("tests/void"));
	BOOST_CHECK(!Dir("tests/void"));

	Dir dir("tests/dir");
	BOOST_REQUIRE(dir);

	size_t count = 0;
	for (const StaticString& entry : dir)
	{
		BOOST_CHECK(entry == "." || entry == ".." || entry == "file");
		++count;
	}
	BOOST_CHECK_EQUAL(count, 3);

	// Reiterability check.
	count = 0;
	for (const StaticString& entry : dir)
	{
		BOOST_CHECK(entry == "." || entry == ".." || entry == "file");
		++count;
	}
	BOOST_CHECK_EQUAL(count, 3);
}
