#include <yttrium/math/point.h>

#include <catch/catch.hpp>

TEST_CASE("point")
{
	using Yttrium::Point;
	{
		Point p;
		CHECK(p._x == 0);
		CHECK(p._y == 0);
	}
	{
		Point p{1, 2};
		CHECK(p._x == 1);
		CHECK(p._y == 2);
	}
}
