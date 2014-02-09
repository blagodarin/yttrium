#include <yttrium/file.h>
#include <yttrium/ion.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(ion_serialization_test)
{
	DECLARE_MEMORY_MANAGER;

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
