#include <Yttrium/vector.h>

#define BOOST_TEST_MODULE vector

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(vector2_test)
{
	Vector2<int> null_vector;

	BOOST_CHECK_EQUAL(null_vector.x, 0);
	BOOST_CHECK_EQUAL(null_vector.y, 0);
}
