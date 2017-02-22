#include <yttrium/static_string.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_static_string_default_construction)
{
	StaticString a;
	BOOST_CHECK_EQUAL(a.size(), 0);
	BOOST_CHECK(a.is_empty());
	BOOST_CHECK(a.text());
	BOOST_CHECK_EQUAL(a[0], '\0');

	StaticString b;
	BOOST_CHECK_EQUAL(b.text(), a.text());
	BOOST_CHECK_EQUAL(b.size(), a.size());
}

BOOST_AUTO_TEST_CASE(test_static_string_construction)
{
	StaticString a("test");
	BOOST_CHECK_EQUAL(a[0], 't');
	BOOST_CHECK_EQUAL(a[1], 'e');
	BOOST_CHECK_EQUAL(a[2], 's');
	BOOST_CHECK_EQUAL(a[3], 't');
	BOOST_CHECK_EQUAL(a[4], '\0');
	BOOST_CHECK_EQUAL(a.size(), 4);

	StaticString b(a);
	BOOST_CHECK_EQUAL(b.text(), a.text());
	BOOST_CHECK_EQUAL(b.size(), a.size());

	const auto& c = "test"_s;
	BOOST_CHECK_EQUAL(c.size(), b.size());
	for (size_t i = 0; i <= c.size(); ++i)
		BOOST_CHECK_EQUAL(c[i], b[i]);
}

BOOST_AUTO_TEST_CASE(test_static_string_comparison)
{
	const auto& ab = "ab"_s;
	BOOST_CHECK(ab == ab);
	BOOST_CHECK(!(ab != ab));
	// cppcheck-suppress duplicateExpression
	BOOST_CHECK(!(ab > ab));
	// cppcheck-suppress duplicateExpression
	BOOST_CHECK(!(ab < ab));
	// cppcheck-suppress duplicateExpression
	BOOST_CHECK(ab >= ab);
	// cppcheck-suppress duplicateExpression
	BOOST_CHECK(ab <= ab);

	const auto& ba = "ba"_s;
	BOOST_CHECK(!(ab == ba));
	BOOST_CHECK(ab != ba);
	BOOST_CHECK(!(ab > ba));
	BOOST_CHECK(ab < ba);
	BOOST_CHECK(!(ab >= ba));
	BOOST_CHECK(ab <= ba);
}

BOOST_AUTO_TEST_CASE(test_static_string_trimmed)
{
	BOOST_CHECK_EQUAL(""_s.trimmed(), ""_s);

	BOOST_CHECK_EQUAL("\x01"_s.trimmed(), ""_s);
	BOOST_CHECK_EQUAL("\x20"_s.trimmed(), ""_s);
	BOOST_CHECK_EQUAL("\x21"_s.trimmed(), "\x21"_s);
	BOOST_CHECK_EQUAL("\x7F"_s.trimmed(), "\x7F"_s);
	BOOST_CHECK_EQUAL("\x80"_s.trimmed(), "\x80"_s);
	BOOST_CHECK_EQUAL("\xFF"_s.trimmed(), "\xFF"_s);

	BOOST_CHECK_EQUAL("\x20\x20"_s.trimmed(), ""_s);
	BOOST_CHECK_EQUAL("\x20\x21\x20"_s.trimmed(), "\x21"_s);
	BOOST_CHECK_EQUAL("\x20\x20\x21\x20\x20"_s.trimmed(), "\x21"_s);
	BOOST_CHECK_EQUAL("\x20\x20\x21\x21\x20\x20"_s.trimmed(), "\x21\x21"_s);
}

BOOST_AUTO_TEST_CASE(test_static_string_to_int)
{
	// Invalid.
	BOOST_CHECK_EQUAL(""_s.to_int32(), 0);
	BOOST_CHECK_EQUAL(""_s.to_uint32(), 0);
	BOOST_CHECK_EQUAL("-1"_s.to_uint32(), 0);
	BOOST_CHECK_EQUAL(""_s.to_int64(), 0);
	BOOST_CHECK_EQUAL(""_s.to_uint64(), 0);
	BOOST_CHECK_EQUAL("-1"_s.to_uint64(), 0);

	// Minimum.
	BOOST_CHECK_EQUAL("-2147483648"_s.to_int32(), -2147483647 - 1);
	BOOST_CHECK_EQUAL("0"_s.to_uint32(), 0);
	BOOST_CHECK_EQUAL("-9223372036854775808"_s.to_int64(), -INT64_C(9223372036854775807) - 1);
	BOOST_CHECK_EQUAL("0"_s.to_uint64(), 0);

	// Maximum.
	BOOST_CHECK_EQUAL("2147483647"_s.to_int32(), 2147483647);
	BOOST_CHECK_EQUAL("+2147483647"_s.to_int32(), 2147483647);
	BOOST_CHECK_EQUAL("4294967295"_s.to_uint32(), UINT32_C(4294967295));
	BOOST_CHECK_EQUAL("+4294967295"_s.to_uint32(), UINT32_C(4294967295));
	BOOST_CHECK_EQUAL("9223372036854775807"_s.to_int64(), INT64_C(9223372036854775807));
	BOOST_CHECK_EQUAL("+9223372036854775807"_s.to_int64(), INT64_C(9223372036854775807));
	BOOST_CHECK_EQUAL("18446744073709551615"_s.to_uint64(), UINT64_C(18446744073709551615));
	BOOST_CHECK_EQUAL("+18446744073709551615"_s.to_uint64(), UINT64_C(18446744073709551615));

	// Overflow.
	BOOST_CHECK_EQUAL("2147483648"_s.to_int32(), -2147483647 - 1);
	BOOST_CHECK_EQUAL("2147483649"_s.to_int32(), -2147483647);
	BOOST_CHECK_EQUAL("4294967296"_s.to_uint32(), 0);
	BOOST_CHECK_EQUAL("4294967297"_s.to_uint32(), 1);
	BOOST_CHECK_EQUAL("9223372036854775808"_s.to_int64(), -INT64_C(9223372036854775807) - 1);
	BOOST_CHECK_EQUAL("9223372036854775809"_s.to_int64(), -INT64_C(9223372036854775807));
	BOOST_CHECK_EQUAL("18446744073709551616"_s.to_uint64(), 0);
	BOOST_CHECK_EQUAL("18446744073709551617"_s.to_uint64(), 1);

	// Underflow.
	BOOST_CHECK_EQUAL("-2147483649"_s.to_int32(), 2147483647);
	BOOST_CHECK_EQUAL("-2147483650"_s.to_int32(), 2147483646);
	BOOST_CHECK_EQUAL("-9223372036854775809"_s.to_int64(), INT64_C(9223372036854775807));
	BOOST_CHECK_EQUAL("-9223372036854775810"_s.to_int64(), INT64_C(9223372036854775806));
}

BOOST_AUTO_TEST_CASE(test_static_string_to_int32_number)
{
	int32_t i = 0;

	BOOST_CHECK(!""_s.to_number(i));

	BOOST_CHECK(!"-2147483649"_s.to_number(i));

	BOOST_CHECK("-2147483648"_s.to_number(i));
	BOOST_CHECK_EQUAL(i, -2147483647 - 1);

	BOOST_CHECK("2147483647"_s.to_number(i));
	BOOST_CHECK_EQUAL(i, 2147483647);

	BOOST_CHECK("+2147483647"_s.to_number(i));
	BOOST_CHECK_EQUAL(i, 2147483647);

	BOOST_CHECK(!"2147483648"_s.to_number(i));
	BOOST_CHECK(!"+2147483648"_s.to_number(i));
}

BOOST_AUTO_TEST_CASE(test_static_string_to_uint32_number)
{
	uint32_t u = 1;

	BOOST_CHECK(!""_s.to_number(u));

	BOOST_CHECK(!"-0"_s.to_number(u));

	BOOST_CHECK("0"_s.to_number(u));
	BOOST_CHECK_EQUAL(u, 0);

	BOOST_CHECK("+0"_s.to_number(u));
	BOOST_CHECK_EQUAL(u, 0);

	BOOST_CHECK("4294967295"_s.to_number(u));
	BOOST_CHECK_EQUAL(u, 4294967295);

	BOOST_CHECK("+4294967295"_s.to_number(u));
	BOOST_CHECK_EQUAL(u, 4294967295);

	BOOST_CHECK(!"4294967296"_s.to_number(u));
	BOOST_CHECK(!"+4294967296"_s.to_number(u));
}

BOOST_AUTO_TEST_CASE(test_static_string_to_double_number)
{
	double d = -1.;

	BOOST_CHECK("0"_s.to_number(d));
	BOOST_CHECK_EQUAL(d, 0.0);

	BOOST_CHECK("0.0"_s.to_number(d));
	BOOST_CHECK_EQUAL(d, 0.0);

	BOOST_CHECK(!"0."_s.to_number(d));

	BOOST_CHECK(!".0"_s.to_number(d));

	BOOST_CHECK("1.0"_s.to_number(d));
	BOOST_CHECK_EQUAL(d, 1.0);

	BOOST_CHECK("3.2"_s.to_number(d));
	BOOST_CHECK_EQUAL(d, 3.2);

	BOOST_CHECK("5.4e1"_s.to_number(d));
	BOOST_CHECK_EQUAL(d, 5.4e1);

	BOOST_CHECK("7.6e+1"_s.to_number(d));
	BOOST_CHECK_EQUAL(d, 7.6e+1);

	BOOST_CHECK("9.8e-1"_s.to_number(d));
	BOOST_CHECK_CLOSE(d, 9.8e-1, 1e-13);

	BOOST_CHECK("+98765.43210e-7"_s.to_number(d));
	BOOST_CHECK_CLOSE(d, +98765.43210e-7, 1e-13);

	BOOST_CHECK("-01234.56789e+7"_s.to_number(d));
	BOOST_CHECK_CLOSE(d, -01234.56789e+7, 1e-13);
}
