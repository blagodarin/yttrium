#include <yttrium/math/vector.h>
#include "iostream.h"

#include <catch2/catch.hpp>

TEST_CASE("vector2")
{
	using Yttrium::Vector2;

	CHECK(Vector2(1, 2) + Vector2(3, 4) == Vector2(4, 6));
	CHECK(Vector2(1, 2) + 3 == Vector2(4, 5));
	CHECK(1 + Vector2(2, 3) == Vector2(3, 4));

	CHECK(-Vector2(1, 2) == Vector2(-1, -2));

	CHECK(Vector2(4, 3) - Vector2(2, 1) == Vector2(2, 2));
	CHECK(Vector2(5, 4) - 3 == Vector2(2, 1));
	CHECK(5 - Vector2(4, 3) == Vector2(1, 2));

	CHECK(Vector2(1, 2) * Vector2(3, 4) == Vector2(3, 8));
	CHECK(Vector2(1, 2) * 3 == Vector2(3, 6));
	CHECK(2 * Vector2(3, 4) == Vector2(6, 8));

	CHECK(Vector2(6, 8) / Vector2(3, 2) == Vector2(2, 4));
	CHECK(Vector2(4, 6) / 2 == Vector2(2, 3));
	CHECK(6 / Vector2(2, 3) == Vector2(3, 2));
}

TEST_CASE("vector3.arithmetics")
{
	using Yttrium::Vector3;

	CHECK(Vector3(1, 2, 3) + Vector3(4, 5, 6) == Vector3(5, 7, 9));
	CHECK(Vector3(1, 2, 3) + 4 == Vector3(5, 6, 7));
	CHECK(1 + Vector3(2, 3, 4) == Vector3(3, 4, 5));

	CHECK(-Vector3(1, 2, 3) == Vector3(-1, -2, -3));

	CHECK(Vector3(6, 5, 4) - Vector3(3, 2, 1) == Vector3(3, 3, 3));
	CHECK(Vector3(6, 5, 4) - 3 == Vector3(3, 2, 1));
	CHECK(7 - Vector3(6, 5, 4) == Vector3(1, 2, 3));

	CHECK(Vector3(1, 2, 3) * Vector3(4, 5, 6) == Vector3(4, 10, 18));
	CHECK(Vector3(1, 2, 3) * 4 == Vector3(4, 8, 12));
	CHECK(4 * Vector3(3, 2, 1) == Vector3(12, 8, 4));

	CHECK(Vector3(1, 4, 9) / Vector3(1, 2, 3) == Vector3(1, 2, 3));
	CHECK(Vector3(2, 4, 6) / 2 == Vector3(1, 2, 3));
	CHECK(4 / Vector3(1, 2, 4) == Vector3(4, 2, 1));
}

TEST_CASE("vector3.length")
{
	CHECK(Yttrium::length({ 1, 4, 8 }) == 9.f);
	CHECK(Yttrium::length(Yttrium::normalize({ 1, 4, 8 })) == 1.f);
}

TEST_CASE("vector4")
{
	using Yttrium::Vector4;

	CHECK(Vector4(1, 2, 3, 4) + Vector4(5, 6, 7, 8) == Vector4(6, 8, 10, 12));
	CHECK(Vector4(1, 2, 3, 4) + 5 == Vector4(6, 7, 8, 9));
	CHECK(1 + Vector4(2, 3, 4, 5) == Vector4(3, 4, 5, 6));

	CHECK(-Vector4(1, 2, 3, 4) == Vector4(-1, -2, -3, -4));

	CHECK(Vector4(8, 7, 6, 5) - Vector4(4, 3, 2, 1) == Vector4(4, 4, 4, 4));
	CHECK(Vector4(9, 8, 7, 6) - 5 == Vector4(4, 3, 2, 1));
	CHECK(9 - Vector4(8, 7, 6, 5) == Vector4(1, 2, 3, 4));

	CHECK(Vector4(1, 2, 3, 4) * Vector4(5, 6, 7, 8) == Vector4(5, 12, 21, 32));
	CHECK(Vector4(1, 2, 3, 4) * 5 == Vector4(5, 10, 15, 20));
	CHECK(5 * Vector4(4, 3, 2, 1) == Vector4(20, 15, 10, 5));

	CHECK(Vector4(1, 4, 9, 16) / Vector4(1, 2, 3, 4) == Vector4(1, 2, 3, 4));
	CHECK(Vector4(2, 4, 6, 8) / 2 == Vector4(1, 2, 3, 4));
	CHECK(8 / Vector4(1, 2, 4, 8) == Vector4(8, 4, 2, 1));
}
