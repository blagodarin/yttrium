#include <yttrium/ion/document.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/temporary_file.h>
#include "iostream.h"

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_ion_serialization)
{
	TemporaryFile file;
	{
		const auto document = IonDocument::open(Reader("tests/ion/original.ion"));
		BOOST_REQUIRE(document);
		document->save(file.name());
		{
			const auto expected = Reader("tests/ion/indented.ion").to_buffer();
			const auto actual = Reader(file).to_buffer();
			BOOST_CHECK_EQUAL(expected, actual);
		}
	}
	{
		const auto document = IonDocument::open(Reader(file.name()));
		BOOST_REQUIRE(document);
		document->save(file.name(), IonDocument::Formatting::Compact);
		{
			const auto expected = Reader("tests/ion/unindented.ion").to_buffer();
			const auto actual = Reader(file).to_buffer();
			BOOST_CHECK_EQUAL(expected, actual);
		}
	}
	{
		const auto document = IonDocument::open(Reader(file.name()));
		BOOST_REQUIRE(document);
		document->save(file.name());
		{
			const auto expected = Reader("tests/ion/indented.ion").to_buffer();
			const auto actual = Reader(file).to_buffer();
			BOOST_CHECK_EQUAL(expected, actual);
		}
	}
}
