#include "iostream.h"

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_point)
{
	BOOST_CHECK_EQUAL(Point(1, 2).x(), 1);
	BOOST_CHECK_EQUAL(Point(1, 2).y(), 2);

	BOOST_CHECK_EQUAL(Point(), Point(0, 0));
}

BOOST_AUTO_TEST_CASE(test_pointf)
{
	BOOST_CHECK_EQUAL(PointF(1, 2).x(), 1);
	BOOST_CHECK_EQUAL(PointF(1, 2).y(), 2);

	BOOST_CHECK_EQUAL(PointF().x(), 0);
	BOOST_CHECK_EQUAL(PointF().y(), 0);

	BOOST_CHECK_EQUAL(PointF(Point(1, 2)).x(), 1);
	BOOST_CHECK_EQUAL(PointF(Point(1, 2)).y(), 2);

	BOOST_CHECK_EQUAL(PointF(1, 2).data()[0], 1);
	BOOST_CHECK_EQUAL(PointF(1, 2).data()[1], 2);

	BOOST_CHECK_EQUAL((PointF(2, 3) / 2).x(), 1);
	BOOST_CHECK_EQUAL((PointF(2, 3) / 2).y(), 1.5);

	BOOST_CHECK_EQUAL((PointF(2, 3) / std::make_pair(4, 3)).x(), .5);
	BOOST_CHECK_EQUAL((PointF(2, 3) / std::make_pair(4, 3)).y(), 1);
}
