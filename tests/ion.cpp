#include <yttrium/ion/document.h>
#include <yttrium/storage/reader.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_ion_serialization)
{
	using Yttrium::IonDocument;
	using Yttrium::Reader;

	BOOST_CHECK(IonDocument::open(Reader("tests/ion/original.ion")));
	BOOST_CHECK(IonDocument::open(Reader("tests/ion/unindented.ion")));
	BOOST_CHECK(IonDocument::open(Reader("tests/ion/indented.ion")));
}
