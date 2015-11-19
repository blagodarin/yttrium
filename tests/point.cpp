#include <yttrium/math/iostream.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_point)
{
	BOOST_CHECK_EQUAL(Point(1, 2).x(), 1);
	BOOST_CHECK_EQUAL(Point(1, 2).y(), 2);

	BOOST_CHECK_EQUAL(Point(), Point(0, 0));
}
