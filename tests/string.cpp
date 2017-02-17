#include <yttrium/string.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_string_initialization)
{
	String s1;

	BOOST_CHECK_EQUAL(s1.size(), 0);
	BOOST_CHECK(s1.is_empty());
	BOOST_CHECK(s1.text());
	BOOST_CHECK_EQUAL(&s1[0], s1.text());
	BOOST_CHECK_EQUAL(s1[0], '\0');

	String s2;

	BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(test_string_assignment)
{
	String s1;
	String s2;
	String s3;

	s1 = "test";

	BOOST_CHECK_EQUAL(s1.size(), 4);
	BOOST_CHECK_EQUAL(&s1[0], s1.text());
	BOOST_CHECK_EQUAL(s1[0], 't');
	BOOST_CHECK_EQUAL(s1[2], 's');
	BOOST_CHECK_EQUAL(s1, "test");

	s2 = s1;

	BOOST_CHECK_EQUAL(s1, s2);

	s3 = s2;
	s2 = "another test";

	BOOST_CHECK_EQUAL(s1, "test");
	BOOST_CHECK_EQUAL(s2, "another test");
	BOOST_CHECK_EQUAL(s3, s1);

	s1[0] = '!';

	BOOST_CHECK_EQUAL(s1, "!est");
	BOOST_CHECK_EQUAL(s3, "test");
}
