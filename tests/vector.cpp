#include <yttrium/vector.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_vector2_arithmetics)
{
	BOOST_CHECK(Vector2(1, 2) + Vector2(3, 4) == Vector2(4, 6));
	BOOST_CHECK(Vector2(1, 2) + 3 == Vector2(4, 5));
	BOOST_CHECK(2 + Vector2(3, 4) == Vector2(5, 6));

	BOOST_CHECK(Vector2(4, 3) - Vector2(2, 1) == Vector2(2, 2));
	BOOST_CHECK(Vector2(4, 3) - 2 == Vector2(2, 1));
	BOOST_CHECK(3 - Vector2(2, 1) == Vector2(1, 2));

	BOOST_CHECK(Vector2(1, 2) * Vector2(3, 4) == Vector2(3, 8));
	BOOST_CHECK(Vector2(1, 2) * 3 == Vector2(3, 6));
	BOOST_CHECK(2 * Vector2(3, 4) == Vector2(6, 8));

	BOOST_CHECK(Vector2(6, 8) / Vector2(3, 2) == Vector2(2, 4));
	BOOST_CHECK(Vector2(4, 6) / 2 == Vector2(2, 3));
	BOOST_CHECK(6 / Vector2(2, 3) == Vector2(3, 2));
}

BOOST_AUTO_TEST_CASE(test_vector4_arithmetics)
{
	BOOST_CHECK(Vector4(1, 2, 3) + Vector4(4, 5, 6) == Vector4(5, 7, 9));
	BOOST_CHECK(Vector4(1, 2, 3) + 4 == Vector4(5, 6, 7));
	BOOST_CHECK(3 + Vector4(4, 5, 6) == Vector4(7, 8, 9));

	BOOST_CHECK(Vector4(6, 5, 4) - Vector4(3, 2, 1) == Vector4(3, 3, 3));
	BOOST_CHECK(Vector4(6, 5, 4) - 3 == Vector4(3, 2, 1));
	BOOST_CHECK(4 - Vector4(3, 2, 1) == Vector4(1, 2, 3));

	BOOST_CHECK(Vector4(1, 2, 3) * Vector4(3, 2, 1) == Vector4(3, 4, 3));
	BOOST_CHECK(Vector4(1, 2, 3) * 3 == Vector4(3, 6, 9));
	BOOST_CHECK(3 * Vector4(3, 2, 1) == Vector4(9, 6, 3));

	BOOST_CHECK(Vector4(8, 4, 2) / Vector4(4, 2, 1) == Vector4(2, 2, 2));
	BOOST_CHECK(Vector4(8, 4, 2) / 2 == Vector4(4, 2, 1));
	BOOST_CHECK(8 / Vector4(4, 2, 1) == Vector4(2, 4, 8));

	BOOST_CHECK(Vector4(3, 3, 3, 3) + 3 == Vector4(6, 6, 6, 1));
	BOOST_CHECK(Vector4(9, 9, 9, 9) - 3 == Vector4(6, 6, 6, 1));
	BOOST_CHECK(Vector4(3, 3, 3, 3) * 3 == Vector4(9, 9, 9, 1));
	BOOST_CHECK(Vector4(9, 9, 9, 9) / 3 == Vector4(3, 3, 3, 1));
}
