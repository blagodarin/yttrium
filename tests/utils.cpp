#include <yttrium/utils.h>

#include <limits>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_is_power_of_2)
{
	static const int table[std::numeric_limits<int8_t>::max() + 1] =
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

	for (int8_t i = std::numeric_limits<int8_t>::min(); i < 0; ++i)
		BOOST_CHECK(!is_power_of_2(i));
	for (uint8_t i = 0; i <= std::numeric_limits<int8_t>::max(); ++i)
		BOOST_CHECK_EQUAL(int{ is_power_of_2(i) }, table[i]);
}

BOOST_AUTO_TEST_CASE(test_next_power_of_2)
{
	static const uint8_t table[std::numeric_limits<int8_t>::max() + 1] =
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

	for (int i = 1; i <= std::numeric_limits<int8_t>::max(); ++i)
		BOOST_CHECK_EQUAL(next_power_of_2(i), table[i]);

	const auto max_power_of_2 = uintmax_t{ std::numeric_limits<intmax_t>::max() } + 1;
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
	BOOST_CHECK(same_sign(std::numeric_limits<intmax_t>::max(), std::numeric_limits<intmax_t>::max()));
	BOOST_CHECK(same_sign(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::min()));
	BOOST_CHECK(!same_sign(std::numeric_limits<intmax_t>::max(), std::numeric_limits<intmax_t>::min()));
	BOOST_CHECK(!same_sign(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max()));
}

BOOST_AUTO_TEST_CASE(test_wrap)
{
	BOOST_CHECK_EQUAL(wrap(-541, -180, 180),  179);
	BOOST_CHECK_EQUAL(wrap(-540, -180, 180), -180);
	BOOST_CHECK_EQUAL(wrap(-361, -180, 180),   -1);
	BOOST_CHECK_EQUAL(wrap(-360, -180, 180),    0);
	BOOST_CHECK_EQUAL(wrap(-359, -180, 180),    1);
	BOOST_CHECK_EQUAL(wrap(-181, -180, 180),  179);
	BOOST_CHECK_EQUAL(wrap(-180, -180, 180), -180);
	BOOST_CHECK_EQUAL(wrap(  -1, -180, 180),   -1);
	BOOST_CHECK_EQUAL(wrap(   0, -180, 180),    0);
	BOOST_CHECK_EQUAL(wrap(   1, -180, 180),    1);
	BOOST_CHECK_EQUAL(wrap( 179, -180, 180),  179);
	BOOST_CHECK_EQUAL(wrap( 180, -180, 180), -180);
	BOOST_CHECK_EQUAL(wrap( 359, -180, 180),   -1);
	BOOST_CHECK_EQUAL(wrap( 360, -180, 180),    0);
	BOOST_CHECK_EQUAL(wrap( 361, -180, 180),    1);
	BOOST_CHECK_EQUAL(wrap( 539, -180, 180),  179);
	BOOST_CHECK_EQUAL(wrap( 540, -180, 180), -180);
}
