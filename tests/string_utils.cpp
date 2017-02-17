#include <yttrium/string_utils.h>

#include <limits>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_make_string)
{
	BOOST_CHECK_EQUAL("", make_string());
	BOOST_CHECK_EQUAL("", make_string(""));
	BOOST_CHECK_EQUAL("", make_string(std::string{}));

	BOOST_CHECK_EQUAL("abc", make_string("abc"));
	BOOST_CHECK_EQUAL("abc", make_string(std::string{ "abc" }));

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

	BOOST_CHECK_EQUAL("-128", make_string(std::numeric_limits<int8_t>::min()));
	BOOST_CHECK_EQUAL("127", make_string(std::numeric_limits<int8_t>::max()));
	BOOST_CHECK_EQUAL("255", make_string(std::numeric_limits<uint8_t>::max()));

	BOOST_CHECK_EQUAL("-32768", make_string(std::numeric_limits<int16_t>::min()));
	BOOST_CHECK_EQUAL("32767", make_string(std::numeric_limits<int16_t>::max()));
	BOOST_CHECK_EQUAL("65535", make_string(std::numeric_limits<uint16_t>::max()));

	BOOST_CHECK_EQUAL("-2147483648", make_string(std::numeric_limits<int32_t>::min()));
	BOOST_CHECK_EQUAL("2147483647", make_string(std::numeric_limits<int32_t>::max()));
	BOOST_CHECK_EQUAL("4294967295", make_string(std::numeric_limits<uint32_t>::max()));

	BOOST_CHECK_EQUAL("-9223372036854775808", make_string(std::numeric_limits<int64_t>::min()));
	BOOST_CHECK_EQUAL("9223372036854775807", make_string(std::numeric_limits<int64_t>::max()));
	BOOST_CHECK_EQUAL("18446744073709551615", make_string(std::numeric_limits<uint64_t>::max()));

	BOOST_CHECK_EQUAL("0", make_string(0.0));
	BOOST_CHECK_EQUAL("7.25", make_string(7.25f));
	BOOST_CHECK_EQUAL("3.625", make_string(3.625));

	BOOST_CHECK_EQUAL("abc123def4.5", make_string("abc", 123, "def", 4.5));
}
