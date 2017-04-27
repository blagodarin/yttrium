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
