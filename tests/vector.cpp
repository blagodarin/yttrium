#include <yttrium/vector.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_vector2)
{
	Vector2<int> null_vector(0);

	BOOST_CHECK_EQUAL(null_vector.x, 0);
	BOOST_CHECK_EQUAL(null_vector.y, 0);
}
