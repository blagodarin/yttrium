#include <Yttrium/file.h>
#include <Yttrium/ion.h>
#include <Yttrium/memory_manager.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(ion_serialization_test)
{
	MemoryManager memory_manager;

	File file(File::Temporary);

	Ion::Document document;

	String expected;
	String actual;

	BOOST_REQUIRE(document.load("tests/ion/original.ion"));

	document.save(file.name());

	BOOST_REQUIRE(File("tests/ion/indented.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);

	BOOST_REQUIRE(document.load(file.name()));

	document.save(file.name(), Ion::DontIndent);

	BOOST_REQUIRE(File("tests/ion/unindented.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);

	BOOST_REQUIRE(document.load(file.name()));

	document.save(file.name());

	BOOST_REQUIRE(File("tests/ion/indented.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}
