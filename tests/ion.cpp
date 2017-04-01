#include <yttrium/ion/document.h>
#include <yttrium/storage/source.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_ion_serialization)
{
	using Yttrium::IonDocument;
	using Yttrium::Source;

	BOOST_CHECK(IonDocument::load(*Source::from("tests/ion/original.ion")));
	BOOST_CHECK(IonDocument::load(*Source::from("tests/ion/unindented.ion")));
	BOOST_CHECK(IonDocument::load(*Source::from("tests/ion/indented.ion")));
}
