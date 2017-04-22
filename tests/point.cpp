#include "iostream.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_point)
{
	using Yttrium::Point;
	{
		Point p;
		BOOST_CHECK_EQUAL(p._x, 0);
		BOOST_CHECK_EQUAL(p._y, 0);
	}
	{
		Point p{1, 2};
		BOOST_CHECK_EQUAL(p._x, 1);
		BOOST_CHECK_EQUAL(p._y, 2);
	}
}

BOOST_AUTO_TEST_CASE(test_pointf)
{
	using Yttrium::PointF;
	{
		const PointF p;
		BOOST_CHECK_EQUAL(p._x, 0);
		BOOST_CHECK_EQUAL(p._y, 0);
	}
	{
		const PointF p{1, 2};
		BOOST_CHECK_EQUAL(p._x, 1);
		BOOST_CHECK_EQUAL(p._y, 2);
	}
	{
		const PointF p{Yttrium::Point{1, 2}};
		BOOST_CHECK_EQUAL(p._x, 1);
		BOOST_CHECK_EQUAL(p._y, 2);
	}
	{
		const auto p = PointF{2, 3} / 2;
		BOOST_CHECK_EQUAL(p._x, 1);
		BOOST_CHECK_EQUAL(p._y, 1.5);
	}
	{
		const auto p = PointF{2, 3} / Yttrium::Vector2{4, 3};
		BOOST_CHECK_EQUAL(p._x, .5);
		BOOST_CHECK_EQUAL(p._y, 1);
	}
}
