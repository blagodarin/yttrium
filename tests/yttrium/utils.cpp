#include <yttrium/utils.h>

#include <limits>

#include <catch2/catch.hpp>

TEST_CASE("utils.is_power_of_2")
{
	using Yttrium::is_power_of_2;

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
		CHECK(!is_power_of_2(i));
	for (uint8_t i = 0; i <= std::numeric_limits<int8_t>::max(); ++i)
		CHECK(int{ is_power_of_2(i) } == table[i]);
}

TEST_CASE("utils.next_power_of_2")
{
	using Yttrium::next_power_of_2;

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
		CHECK(next_power_of_2(i) == table[i]);

	const auto max_power_of_2 = static_cast<uintmax_t>(std::numeric_limits<intmax_t>::max()) + 1;
	CHECK(next_power_of_2(max_power_of_2 - 1) == max_power_of_2);
	CHECK(next_power_of_2(max_power_of_2) == max_power_of_2);
	CHECK(next_power_of_2(max_power_of_2 + 1) != max_power_of_2);
}

TEST_CASE("utils.same_sign")
{
	using Yttrium::same_sign;

	CHECK(same_sign<intmax_t>(0, 0));
	CHECK(same_sign<intmax_t>(-1, -1));
	CHECK(!same_sign<intmax_t>(0, -1));
	CHECK(!same_sign<intmax_t>(-1, 0));
	CHECK(same_sign(std::numeric_limits<intmax_t>::max(), std::numeric_limits<intmax_t>::max()));
	CHECK(same_sign(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::min()));
	CHECK(!same_sign(std::numeric_limits<intmax_t>::max(), std::numeric_limits<intmax_t>::min()));
	CHECK(!same_sign(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max()));
}

TEST_CASE("utils.wrap")
{
	using Yttrium::wrap;

	CHECK(wrap(-541, -180, 180) ==  179);
	CHECK(wrap(-540, -180, 180) == -180);
	CHECK(wrap(-361, -180, 180) ==   -1);
	CHECK(wrap(-360, -180, 180) ==    0);
	CHECK(wrap(-359, -180, 180) ==    1);
	CHECK(wrap(-181, -180, 180) ==  179);
	CHECK(wrap(-180, -180, 180) == -180);
	CHECK(wrap(  -1, -180, 180) ==   -1);
	CHECK(wrap(   0, -180, 180) ==    0);
	CHECK(wrap(   1, -180, 180) ==    1);
	CHECK(wrap( 179, -180, 180) ==  179);
	CHECK(wrap( 180, -180, 180) == -180);
	CHECK(wrap( 359, -180, 180) ==   -1);
	CHECK(wrap( 360, -180, 180) ==    0);
	CHECK(wrap( 361, -180, 180) ==    1);
	CHECK(wrap( 539, -180, 180) ==  179);
	CHECK(wrap( 540, -180, 180) == -180);
}
