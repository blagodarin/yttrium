#include <Yttrium/file.h>
#include <Yttrium/ion.h>
#include <Yttrium/memory_manager.h>

#define BOOST_TEST_MODULE Ion tests

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_FIXTURE_TEST_SUITE(test_suite_memory_manager, MemoryManager)

BOOST_AUTO_TEST_CASE(serialization_test)
{
	Ion::Document document;

	String expected;
	String actual;

	BOOST_REQUIRE(document.load("tests/ion/original.ion"));

	document.save("tmp.ion");

	BOOST_REQUIRE(File("tests/ion/indented.ion").read_all(&expected));
	BOOST_REQUIRE(File("tmp.ion").read_all(&actual));
	BOOST_CHECK(expected == actual);

	BOOST_REQUIRE(document.load("tmp.ion"));

	document.save("tmp.ion", Ion::DontIndent);

	BOOST_REQUIRE(File("tests/ion/unindented.ion").read_all(&expected));
	BOOST_REQUIRE(File("tmp.ion").read_all(&actual));
	BOOST_CHECK(expected == actual);

	BOOST_REQUIRE(document.load("tmp.ion"));

	document.save("tmp.ion");

	BOOST_REQUIRE(File("tests/ion/indented.ion").read_all(&expected));
	BOOST_REQUIRE(File("tmp.ion").read_all(&actual));
	BOOST_CHECK(expected == actual);
}

BOOST_AUTO_TEST_SUITE_END()
