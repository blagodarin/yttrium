#include <Yttrium/static_string.h>

#define BOOST_TEST_MODULE static_string

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(initialization_test)
{
	StaticString s1;

	BOOST_CHECK_EQUAL(s1.size(), 0);
	BOOST_CHECK(s1.is_empty());
	BOOST_CHECK(s1.text() != nullptr);
	BOOST_CHECK_EQUAL(&s1[0], s1.text());
	BOOST_CHECK_EQUAL(s1[0], 0);

	StaticString s2;

	BOOST_CHECK(s1 == s2);
	BOOST_CHECK_EQUAL(s1.text(), s2.text());
	BOOST_CHECK_EQUAL(s1.size(), s2.size());
}

BOOST_AUTO_TEST_CASE(assignment_test)
{
	StaticString s1 = "test";

	BOOST_CHECK_EQUAL(s1.size(), 4);
	BOOST_CHECK_EQUAL(&s1[0], s1.text());
	BOOST_CHECK_EQUAL(s1[0], 't');
	BOOST_CHECK_EQUAL(s1[2], 's');
	BOOST_CHECK(s1 == "test");

	StaticString s2 = s1;

	BOOST_CHECK(s1 == s2);
	BOOST_CHECK_EQUAL(s1.text(), s2.text());
	BOOST_CHECK_EQUAL(s1.size(), s2.size());
}

BOOST_AUTO_TEST_CASE(to_int_test)
{
	BOOST_CHECK_EQUAL(StaticString("-2147483648").to_int32(), -2147483647 - 1);
	BOOST_CHECK_EQUAL(StaticString("2147483647").to_int32(), 2147483647);
	BOOST_CHECK_EQUAL(StaticString("+2147483647").to_int32(), 2147483647);

	BOOST_CHECK_EQUAL(StaticString("-1").to_uint32(), 0);
	BOOST_CHECK_EQUAL(StaticString("0").to_uint32(), 0);
	BOOST_CHECK_EQUAL(StaticString("4294967295").to_uint32(), UINT32_C(4294967295));
	BOOST_CHECK_EQUAL(StaticString("+4294967295").to_uint32(), UINT32_C(4294967295));

	BOOST_CHECK_EQUAL(StaticString("-9223372036854775808").to_int64(), -INT64_C(9223372036854775807) - 1);
	BOOST_CHECK_EQUAL(StaticString("9223372036854775807").to_int64(), INT64_C(9223372036854775807));
	BOOST_CHECK_EQUAL(StaticString("+9223372036854775807").to_int64(), INT64_C(9223372036854775807));

	BOOST_CHECK_EQUAL(StaticString("-1").to_uint64(), 0);
	BOOST_CHECK_EQUAL(StaticString("0").to_uint64(), 0);
	BOOST_CHECK_EQUAL(StaticString("18446744073709551615").to_uint64(), UINT64_C(18446744073709551615));
	BOOST_CHECK_EQUAL(StaticString("+18446744073709551615").to_uint64(), UINT64_C(18446744073709551615));
}

BOOST_AUTO_TEST_CASE(to_int32_number_test)
{
	int32_t i32;

	BOOST_CHECK(StaticString("2147483647").to_number(&i32));
	BOOST_CHECK_EQUAL(i32, 2147483647);

	BOOST_CHECK(!StaticString("2147483648").to_number(&i32));

	BOOST_CHECK(StaticString("+2147483647").to_number(&i32));
	BOOST_CHECK_EQUAL(i32, 2147483647);

	BOOST_CHECK(!StaticString("+2147483648").to_number(&i32));

	BOOST_CHECK(StaticString("-2147483648").to_number(&i32));
	BOOST_CHECK_EQUAL(i32, -2147483647 - 1);

	BOOST_CHECK(!StaticString("-2147483649").to_number(&i32));
}

BOOST_AUTO_TEST_CASE(to_double_number_test)
{
	double d;

	BOOST_CHECK(StaticString("0").to_number(&d));
	BOOST_CHECK_EQUAL(d, 0.0);

	BOOST_CHECK(StaticString("0.0").to_number(&d));
	BOOST_CHECK_EQUAL(d, 0.0);

	BOOST_CHECK(!StaticString("0.").to_number(&d));

	BOOST_CHECK(!StaticString(".0").to_number(&d));

	BOOST_CHECK(StaticString("1.0").to_number(&d));
	BOOST_CHECK_EQUAL(d, 1.0);

	BOOST_CHECK(StaticString("3.2").to_number(&d));
	BOOST_CHECK_EQUAL(d, 3.2);

	BOOST_CHECK(StaticString("5.4e1").to_number(&d));
	BOOST_CHECK_EQUAL(d, 5.4e1);

	BOOST_CHECK(StaticString("7.6e+1").to_number(&d));
	BOOST_CHECK_EQUAL(d, 7.6e+1);

	BOOST_CHECK(StaticString("9.8e-1").to_number(&d));
	BOOST_CHECK_CLOSE(d, 9.8e-1, 1e-13);

	BOOST_CHECK(StaticString("+98765.43210e-7").to_number(&d));
	BOOST_CHECK_CLOSE(d, +98765.43210e-7, 1e-13);

	BOOST_CHECK(StaticString("-01234.56789e+7").to_number(&d));
	BOOST_CHECK_CLOSE(d, -01234.56789e+7, 1e-13);
}