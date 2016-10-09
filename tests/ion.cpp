#include <yttrium/io/file.h>
#include <yttrium/io/reader.h>
#include <yttrium/ion/document.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/static_string.h>

#include <boost/test/unit_test.hpp>

namespace Yttrium
{
	inline std::ostream& operator<<(std::ostream& stream, const Buffer& buffer)
	{
		return stream << "Buffer(" << buffer.size() << ")";
	}
}

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_ion_serialization)
{
	File file(File::Temporary);

	IonDocument document;

	BOOST_REQUIRE(document.load("tests/ion/original.ion"));

	document.save(file.name());
	{
		const auto expected = Reader("tests/ion/indented.ion").to_buffer();
		const auto actual = Reader(file.name()).to_buffer();
		BOOST_CHECK_EQUAL(expected, actual);
	}

	BOOST_REQUIRE(document.load(file.name()));

	document.save(file.name(), IonDocument::Formatting::Compact);
	{
		const auto expected = Reader("tests/ion/unindented.ion").to_buffer();
		const auto actual = Reader(file.name()).to_buffer();
		BOOST_CHECK_EQUAL(expected, actual);
	}

	BOOST_REQUIRE(document.load(file.name()));

	document.save(file.name());
	{
		const auto expected = Reader("tests/ion/indented.ion").to_buffer();
		const auto actual = Reader(file.name()).to_buffer();
		BOOST_CHECK_EQUAL(expected, actual);
	}
}
