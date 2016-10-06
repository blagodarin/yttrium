#include <yttrium/io/file.h>
#include <yttrium/ion/document.h>
#include <yttrium/string.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_ion_serialization)
{
	File file(File::Temporary);

	IonDocument document;

	String expected;
	String actual;

	BOOST_REQUIRE(document.load("tests/ion/original.ion"));

	document.save(file.name());
	BOOST_REQUIRE(File("tests/ion/indented.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);

	BOOST_REQUIRE(document.load(file.name()));

	document.save(file.name(), IonDocument::Formatting::Compact);
	BOOST_REQUIRE(File("tests/ion/unindented.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);

	BOOST_REQUIRE(document.load(file.name()));

	document.save(file.name());
	BOOST_REQUIRE(File("tests/ion/indented.ion").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}
