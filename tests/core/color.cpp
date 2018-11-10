#include <yttrium/math/color.h>

#include <catch2/catch.hpp>

TEST_CASE("color.bgra32")
{
	using Yttrium::Bgra32;
	{
		Bgra32 color{1, 2, 3};
		CHECK(color._b == 1u);
		CHECK(color._g == 2u);
		CHECK(color._r == 3u);
		CHECK(color._a == 255u);
	}
	{
		Bgra32 color{1, 2, 3, 4};
		CHECK(color._b == 1u);
		CHECK(color._g == 2u);
		CHECK(color._r == 3u);
		CHECK(color._a == 4u);
	}
	CHECK(Bgra32(1, 2, 3) == Bgra32(1, 2, 3, 0xff));
	CHECK(Bgra32(1, 2, 3, 4) != Bgra32(1, 2, 3, 0xff));
}