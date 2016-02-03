#include <yttrium/utils.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_is_power_of_2)
{
	static const int table[INT8_MAX + 1] =
	{
		0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	for (int8_t i = INT8_MIN; i < 0; ++i)
		BOOST_CHECK(!is_power_of_2(i));
	for (uint8_t i = 0; i <= INT8_MAX; ++i)
		BOOST_CHECK_EQUAL(is_power_of_2(i), table[i]);
}

BOOST_AUTO_TEST_CASE(test_max)
{
	BOOST_CHECK_EQUAL(max(INTMAX_MIN, INTMAX_MAX), INTMAX_MAX);
	BOOST_CHECK_EQUAL(max(uintmax_t{0}, UINTMAX_MAX), UINTMAX_MAX);
}

BOOST_AUTO_TEST_CASE(test_min)
{
	BOOST_CHECK_EQUAL(min(INTMAX_MIN, INTMAX_MAX), INTMAX_MIN);
	BOOST_CHECK_EQUAL(min(uintmax_t{0}, UINTMAX_MAX), 0);
}

BOOST_AUTO_TEST_CASE(test_next_power_of_2)
{
	static const uint8_t table[INT8_MAX + 1] =
	{
		0, 1, 2, 4, 4, 8, 8, 8, 8, 16, 16, 16, 16, 16, 16, 16,
		16, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
		32, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
	};

	for (uint8_t i = 1; i <= INT8_MAX; ++i)
		BOOST_CHECK_EQUAL(next_power_of_2(i), table[i]);

	const auto max_power_of_2 = uintmax_t{INTMAX_MAX} + 1;
	BOOST_CHECK_EQUAL(next_power_of_2(max_power_of_2 - 1), max_power_of_2);
	BOOST_CHECK_EQUAL(next_power_of_2(max_power_of_2), max_power_of_2);
	BOOST_CHECK(next_power_of_2(max_power_of_2 + 1) != max_power_of_2);
}

BOOST_AUTO_TEST_CASE(test_same_sign)
{
	BOOST_CHECK(same_sign<intmax_t>(0, 0));
	BOOST_CHECK(same_sign<intmax_t>(-1, -1));
	BOOST_CHECK(!same_sign<intmax_t>(0, -1));
	BOOST_CHECK(!same_sign<intmax_t>(-1, 0));
	BOOST_CHECK(same_sign(INTMAX_MAX, INTMAX_MAX));
	BOOST_CHECK(same_sign(INTMAX_MIN, INTMAX_MIN));
	BOOST_CHECK(!same_sign(INTMAX_MAX, INTMAX_MIN));
	BOOST_CHECK(!same_sign(INTMAX_MIN, INTMAX_MAX));
}
