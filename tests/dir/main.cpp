#include <Yttrium/dir.h>
#include <Yttrium/memory_manager.h>

#define BOOST_TEST_MODULE dir

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_FIXTURE_TEST_SUITE(test_suite_memory_manager, MemoryManager)

BOOST_AUTO_TEST_CASE(dir_test)
{
	Dir dir;

	BOOST_CHECK(!dir.is_opened());

	BOOST_REQUIRE(dir.open("tests/dir"));

	BOOST_CHECK(dir.is_opened());

	size_t count = 0;

	for (const StaticString &entry: dir)
	{
		BOOST_CHECK(entry == "." || entry == ".." || entry == "main.cpp");
		++count;
	}

	BOOST_CHECK(count == 3);

	// Reiterability check.

	count = 0;

	for (const StaticString &entry: dir)
	{
		BOOST_CHECK(entry == "." || entry == ".." || entry == "main.cpp");
		++count;
	}

	BOOST_CHECK(count == 3);
}

BOOST_AUTO_TEST_SUITE_END()
