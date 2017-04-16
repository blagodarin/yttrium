#include <yttrium/math/vector2.h>
#include <yttrium/math/vector3.h>
#include <yttrium/math/vector4.h>

#include <boost/test/unit_test.hpp>

namespace
{
	bool operator==(const Yttrium::Vector2& a, const Yttrium::Vector2& b) { return a.x == b.x && a.y == b.y; }
	bool operator==(const Yttrium::Vector3& a, const Yttrium::Vector3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
	bool operator==(const Yttrium::Vector4& a, const Yttrium::Vector4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
}

BOOST_AUTO_TEST_CASE(test_vector2_arithmetics)
{
	using Yttrium::Vector2;

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

BOOST_AUTO_TEST_CASE(test_vector3)
{
	using Yttrium::Vector3;

	BOOST_CHECK_EQUAL(Vector3(1, 4, 8).length(), 9);
	BOOST_CHECK_EQUAL(Vector3(1, 4, 8).normalized().length(), 1);
}

BOOST_AUTO_TEST_CASE(test_vector3_arithmetics)
{
	using Yttrium::Vector3;

	BOOST_CHECK(Vector3(1, 2, 3) + Vector3(4, 5, 6) == Vector3(5, 7, 9));
	BOOST_CHECK(Vector3(1, 2, 3) + 4 == Vector3(5, 6, 7));
	BOOST_CHECK(3 + Vector3(4, 5, 6) == Vector3(7, 8, 9));

	BOOST_CHECK(-Vector3(1, 2, 3) == Vector3(-1, -2, -3));

	BOOST_CHECK(Vector3(6, 5, 4) - Vector3(3, 2, 1) == Vector3(3, 3, 3));
	BOOST_CHECK(Vector3(6, 5, 4) - 3 == Vector3(3, 2, 1));
	BOOST_CHECK(4 - Vector3(3, 2, 1) == Vector3(1, 2, 3));

	BOOST_CHECK(Vector3(1, 2, 3) * Vector3(3, 2, 1) == Vector3(3, 4, 3));
	BOOST_CHECK(Vector3(1, 2, 3) * 3 == Vector3(3, 6, 9));
	BOOST_CHECK(3 * Vector3(3, 2, 1) == Vector3(9, 6, 3));

	BOOST_CHECK(Vector3(8, 4, 2) / Vector3(4, 2, 1) == Vector3(2, 2, 2));
	BOOST_CHECK(Vector3(8, 4, 2) / 2 == Vector3(4, 2, 1));
	BOOST_CHECK(8 / Vector3(4, 2, 1) == Vector3(2, 4, 8));
}

BOOST_AUTO_TEST_CASE(test_vector4)
{
	using Yttrium::Vector4;

	BOOST_CHECK_EQUAL(Vector4(1, 4, 8).length(), 9);
	BOOST_CHECK_EQUAL(Vector4(1, 4, 8).normalized().length(), 1);
}

BOOST_AUTO_TEST_CASE(test_vector4_arithmetics)
{
	using Yttrium::Vector4;

	BOOST_CHECK(Vector4(1, 2, 3) + Vector4(4, 5, 6) == Vector4(5, 7, 9));
	BOOST_CHECK(Vector4(1, 2, 3) + 4 == Vector4(5, 6, 7));
	BOOST_CHECK(3 + Vector4(4, 5, 6) == Vector4(7, 8, 9));

	BOOST_CHECK(-Vector4(1, 2, 3) == Vector4(-1, -2, -3));

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
