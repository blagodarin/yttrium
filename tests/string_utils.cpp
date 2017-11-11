#include <yttrium/string_utils.h>

#include <limits>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_make_string)
{
	using Yttrium::make_string;

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

BOOST_AUTO_TEST_CASE(test_string_ends_with)
{
	using namespace Yttrium::strings;

	BOOST_CHECK(ends_with("", ""));
	BOOST_CHECK(ends_with("test", ""));
	BOOST_CHECK(ends_with("test", "t"));
	BOOST_CHECK(!ends_with("test", "tes"));
	BOOST_CHECK(ends_with("test", "test"));
	BOOST_CHECK(!ends_with("test", "test "));
}

BOOST_AUTO_TEST_CASE(test_string_to_int)
{
	using namespace Yttrium::strings;

	// Invalid.
	BOOST_CHECK_EQUAL(to_int(""), 0);
	BOOST_CHECK_EQUAL(to_int32(""), 0);
	BOOST_CHECK_EQUAL(to_uint32(""), 0u);
	BOOST_CHECK_EQUAL(to_uint32("-1"), 0u);
	BOOST_CHECK_EQUAL(to_int64(""), 0);
	BOOST_CHECK_EQUAL(to_uint64(""), 0u);
	BOOST_CHECK_EQUAL(to_uint64("-1"), 0u);

	// Minimum.
	BOOST_CHECK_EQUAL(to_int(std::to_string(std::numeric_limits<int>::min())), std::numeric_limits<int>::min());
	BOOST_CHECK_EQUAL(to_int32("-2147483648"), -2147483647 - 1);
	BOOST_CHECK_EQUAL(to_uint32("0"), 0u);
	BOOST_CHECK_EQUAL(to_int64("-9223372036854775808"), -INT64_C(9223372036854775807) - 1);
	BOOST_CHECK_EQUAL(to_uint64("0"), 0u);

	// Maximum.
	BOOST_CHECK_EQUAL(to_int(std::to_string(std::numeric_limits<int>::max())), std::numeric_limits<int>::max());
	BOOST_CHECK_EQUAL(to_int32("2147483647"), 2147483647);
	BOOST_CHECK_EQUAL(to_int32("+2147483647"), 2147483647);
	BOOST_CHECK_EQUAL(to_uint32("4294967295"), UINT32_C(4294967295));
	BOOST_CHECK_EQUAL(to_uint32("+4294967295"), UINT32_C(4294967295));
	BOOST_CHECK_EQUAL(to_int64("9223372036854775807"), INT64_C(9223372036854775807));
	BOOST_CHECK_EQUAL(to_int64("+9223372036854775807"), INT64_C(9223372036854775807));
	BOOST_CHECK_EQUAL(to_uint64("18446744073709551615"), UINT64_C(18446744073709551615));
	BOOST_CHECK_EQUAL(to_uint64("+18446744073709551615"), UINT64_C(18446744073709551615));

	// Overflow.
	BOOST_CHECK_EQUAL(to_int32("2147483648"), -2147483647 - 1);
	BOOST_CHECK_EQUAL(to_int32("2147483649"), -2147483647);
	BOOST_CHECK_EQUAL(to_uint32("4294967296"), 0u);
	BOOST_CHECK_EQUAL(to_uint32("4294967297"), 1u);
	BOOST_CHECK_EQUAL(to_int64("9223372036854775808"), -INT64_C(9223372036854775807) - 1);
	BOOST_CHECK_EQUAL(to_int64("9223372036854775809"), -INT64_C(9223372036854775807));
	BOOST_CHECK_EQUAL(to_uint64("18446744073709551616"), 0u);
	BOOST_CHECK_EQUAL(to_uint64("18446744073709551617"), 1u);

	// Underflow.
	BOOST_CHECK_EQUAL(to_int32("-2147483649"), 2147483647);
	BOOST_CHECK_EQUAL(to_int32("-2147483650"), 2147483646);
	BOOST_CHECK_EQUAL(to_int64("-9223372036854775809"), INT64_C(9223372036854775807));
	BOOST_CHECK_EQUAL(to_int64("-9223372036854775810"), INT64_C(9223372036854775806));
}

BOOST_AUTO_TEST_CASE(test_string_to_float)
{
	using namespace Yttrium::strings;

	BOOST_CHECK_EQUAL(to_float(""), 0.f);
	BOOST_CHECK_EQUAL(to_double(""), 0.);

	BOOST_CHECK_EQUAL(to_float("1"), 1.f);
	BOOST_CHECK_EQUAL(to_double("1"), 1.);

	BOOST_CHECK_EQUAL(to_float("0.5"), .5f);
	BOOST_CHECK_EQUAL(to_double("0.5"), .5);

	BOOST_CHECK_EQUAL(to_float("-0.25"), -.25f);
	BOOST_CHECK_EQUAL(to_double("-0.25"), -.25);

	BOOST_CHECK_EQUAL(to_float("+0.125"), .125f);
	BOOST_CHECK_EQUAL(to_double("+0.125"), .125);

	BOOST_CHECK_CLOSE(to_float("+98765.43210e-7"), +98765.43210e-7f, 1e-13f);
	BOOST_CHECK_CLOSE(to_double("+98765.43210e-7"), +98765.43210e-7, 1e-13);

	BOOST_CHECK_CLOSE(to_float("-01234.56789e+7"), -01234.56789e+7f, 1e-13f);
	BOOST_CHECK_CLOSE(to_double("-01234.56789e+7"), -01234.56789e+7, 1e-13);
}

BOOST_AUTO_TEST_CASE(test_string_to_number_int32)
{
	using namespace Yttrium::strings;

	int32_t i = 0;

	BOOST_CHECK(!to_number("-9999999999", i));
	BOOST_CHECK(!to_number("-2147483649", i));

	BOOST_CHECK(to_number("-2147483648", i));
	BOOST_CHECK_EQUAL(i, -2147483647 - 1);

	BOOST_CHECK(to_number("2147483647", i));
	BOOST_CHECK_EQUAL(i, 2147483647);

	BOOST_CHECK(to_number("+2147483647", i));
	BOOST_CHECK_EQUAL(i, 2147483647);

	BOOST_CHECK(!to_number("2147483648", i));
	BOOST_CHECK(!to_number("+2147483648", i));
	BOOST_CHECK(!to_number("9999999999", i));
	BOOST_CHECK(!to_number("+9999999999", i));

	BOOST_CHECK(!to_number("1 ", i));
	BOOST_CHECK(!to_number(" 1", i));

	BOOST_CHECK(!to_number("", i));
	BOOST_CHECK(!to_number("+", i));
	BOOST_CHECK(!to_number("-", i));
	BOOST_CHECK(!to_number("A", i));
}

BOOST_AUTO_TEST_CASE(test_string_to_number_uint32)
{
	using namespace Yttrium::strings;

	uint32_t u = 1;

	BOOST_CHECK(!to_number("-0", u));

	BOOST_CHECK(to_number("0", u));
	BOOST_CHECK_EQUAL(u, 0);

	BOOST_CHECK(to_number("+0", u));
	BOOST_CHECK_EQUAL(u, 0);

	BOOST_CHECK(to_number("4294967295", u));
	BOOST_CHECK_EQUAL(u, 4294967295u);

	BOOST_CHECK(to_number("+4294967295", u));
	BOOST_CHECK_EQUAL(u, 4294967295u);

	BOOST_CHECK(!to_number("4294967296", u));
	BOOST_CHECK(!to_number("+4294967296", u));
	BOOST_CHECK(!to_number("9999999999", u));
	BOOST_CHECK(!to_number("+9999999999", u));

	BOOST_CHECK(!to_number("1 ", u));
	BOOST_CHECK(!to_number(" 1", u));

	BOOST_CHECK(!to_number("", u));
	BOOST_CHECK(!to_number("+", u));
	BOOST_CHECK(!to_number("A", u));
}

BOOST_AUTO_TEST_CASE(test_string_to_number_double)
{
	using namespace Yttrium::strings;

	double d = -1.;

	BOOST_CHECK(to_number("0", d));
	BOOST_CHECK_EQUAL(d, 0.0);

	BOOST_CHECK(to_number("0.0", d));
	BOOST_CHECK_EQUAL(d, 0.0);

	BOOST_CHECK(!to_number("0.", d));

	BOOST_CHECK(!to_number(".0", d));

	BOOST_CHECK(to_number("1.0", d));
	BOOST_CHECK_EQUAL(d, 1.0);

	BOOST_CHECK(to_number("3.2", d));
	BOOST_CHECK_EQUAL(d, 3.2);

	BOOST_CHECK(to_number("5.4e1", d));
	BOOST_CHECK_EQUAL(d, 5.4e1);

	BOOST_CHECK(to_number("7.6e+1", d));
	BOOST_CHECK_EQUAL(d, 7.6e+1);

	BOOST_CHECK(to_number("9.8e-1", d));
	BOOST_CHECK_CLOSE(d, 9.8e-1, 1e-13);

	BOOST_CHECK(to_number("+98765.43210e-7", d));
	BOOST_CHECK_CLOSE(d, +98765.43210e-7, 1e-13);

	BOOST_CHECK(to_number("-01234.56789e+7", d));
	BOOST_CHECK_CLOSE(d, -01234.56789e+7, 1e-13);

	BOOST_CHECK(!to_number("1 ", d));
	BOOST_CHECK(!to_number(" 1", d));

	BOOST_CHECK(!to_number("", d));
	BOOST_CHECK(!to_number("1e", d));
	BOOST_CHECK(!to_number("1eA", d));
}

BOOST_AUTO_TEST_CASE(test_string_to_time)
{
	using namespace Yttrium::strings;

	BOOST_CHECK_EQUAL(to_time(""), 0);
	BOOST_CHECK_EQUAL(to_time("1"), 1);
	BOOST_CHECK_EQUAL(to_time("1:2"), 62);
	BOOST_CHECK_EQUAL(to_time("1:1:2"), 3662);
	BOOST_CHECK_EQUAL(to_time("1:1:2.5"), 3662.5);
	BOOST_CHECK_EQUAL(to_time("-10:1:2.25"), -36062.25);
	BOOST_CHECK_EQUAL(to_time("+101:1:2.125"), 363662.125);
}
