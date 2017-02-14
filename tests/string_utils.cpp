#include <yttrium/string_utils.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_make_string)
{
	BOOST_CHECK_EQUAL("", make_string());
	BOOST_CHECK_EQUAL("", make_string(""));
	BOOST_CHECK_EQUAL("", make_string(std::string{}));
	BOOST_CHECK_EQUAL("", make_string(StaticString{}));
	BOOST_CHECK_EQUAL("", make_string(""_s));

	BOOST_CHECK_EQUAL("abc", make_string("abc"));
	BOOST_CHECK_EQUAL("abc", make_string(std::string{ "abc" }));
	BOOST_CHECK_EQUAL("abc", make_string("abc"_s));

	BOOST_CHECK_EQUAL("0", make_string(short{ 0 }));
	BOOST_CHECK_EQUAL("0", make_string(static_cast<unsigned short>(0)));
	BOOST_CHECK_EQUAL("0", make_string(0));
	BOOST_CHECK_EQUAL("0", make_string(0u));
	BOOST_CHECK_EQUAL("0", make_string(0l));
	BOOST_CHECK_EQUAL("0", make_string(0ul));
	BOOST_CHECK_EQUAL("0", make_string(0ll));
	BOOST_CHECK_EQUAL("0", make_string(0ull));

	BOOST_CHECK_EQUAL("a", make_string('a'));
	BOOST_CHECK_EQUAL("97", make_string(static_cast<signed char>('a')));
	BOOST_CHECK_EQUAL("97", make_string(static_cast<unsigned char>('a')));

	BOOST_CHECK_EQUAL("0", make_string(0.0));
	BOOST_CHECK_EQUAL("7.25", make_string(7.25f));
	BOOST_CHECK_EQUAL("3.625", make_string(3.625));

	BOOST_CHECK_EQUAL("abc123def4.5", make_string("abc", 123, "def", 4.5));
}
