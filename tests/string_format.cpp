#include <yttrium/string.h>
#include <yttrium/string_format.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_string_format)
{
	BOOST_CHECK_EQUAL(String() << "", "");
	BOOST_CHECK_EQUAL(String() << "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	BOOST_CHECK_EQUAL(String() << "AB" << "CD" << "EF", "ABCDEF");

	BOOST_CHECK_EQUAL(String() << short{0}, "0");
	BOOST_CHECK_EQUAL(String() << static_cast<unsigned short>(0), "0");
	BOOST_CHECK_EQUAL(String() << 0, "0");
	BOOST_CHECK_EQUAL(String() << 0u, "0");
	BOOST_CHECK_EQUAL(String() << 0l, "0");
	BOOST_CHECK_EQUAL(String() << 0ul, "0");
	BOOST_CHECK_EQUAL(String() << 0ll, "0");
	BOOST_CHECK_EQUAL(String() << 0ull, "0");

	BOOST_CHECK_EQUAL(String() << 'A', "A");
	BOOST_CHECK_EQUAL(String() << static_cast<signed char>('A'), "65");
	BOOST_CHECK_EQUAL(String() << static_cast<unsigned char>('A'), "65");

	BOOST_CHECK_EQUAL(String() << 0.0, "0");
	BOOST_CHECK_EQUAL(String() << 7.25f, "7.25");
	BOOST_CHECK_EQUAL(String() << 3.625, "3.625");

	BOOST_CHECK_EQUAL(String() << repeat('A', 0), "");
	BOOST_CHECK_EQUAL(String() << repeat('A', 1), "A");
	BOOST_CHECK_EQUAL(String() << repeat('A', 2), "AA");
}
