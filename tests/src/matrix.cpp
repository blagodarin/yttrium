#include "iostream.h"

#include <boost/test/unit_test.hpp>

namespace Yttrium // Anonymous namespace breaks compilation.
{
	constexpr bool operator==(const Matrix4& a, const Matrix4& b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z && a.t == b.t; }
}

using Yttrium::Matrix4;

BOOST_AUTO_TEST_CASE(matrix4_construction)
{
	const Matrix4 m
	{
		11, 12, 13, 14,
		21, 22, 23, 24,
		31, 32, 33, 34,
		41, 42, 43, 44,
	};

	BOOST_CHECK_EQUAL(m.x.x, 11.f);
	BOOST_CHECK_EQUAL(m.y.x, 12.f);
	BOOST_CHECK_EQUAL(m.z.x, 13.f);
	BOOST_CHECK_EQUAL(m.t.x, 14.f);
	BOOST_CHECK_EQUAL(m.x.y, 21.f);
	BOOST_CHECK_EQUAL(m.y.y, 22.f);
	BOOST_CHECK_EQUAL(m.z.y, 23.f);
	BOOST_CHECK_EQUAL(m.t.y, 24.f);
	BOOST_CHECK_EQUAL(m.x.z, 31.f);
	BOOST_CHECK_EQUAL(m.y.z, 32.f);
	BOOST_CHECK_EQUAL(m.z.z, 33.f);
	BOOST_CHECK_EQUAL(m.t.z, 34.f);
	BOOST_CHECK_EQUAL(m.x.w, 41.f);
	BOOST_CHECK_EQUAL(m.y.w, 42.f);
	BOOST_CHECK_EQUAL(m.z.w, 43.f);
	BOOST_CHECK_EQUAL(m.t.w, 44.f);

	BOOST_CHECK_EQUAL((&m.x.x)[0], 11.f);
	BOOST_CHECK_EQUAL((&m.x.x)[1], 21.f);
	BOOST_CHECK_EQUAL((&m.x.x)[2], 31.f);
	BOOST_CHECK_EQUAL((&m.x.x)[3], 41.f);
	BOOST_CHECK_EQUAL((&m.x.x)[4], 12.f);
	BOOST_CHECK_EQUAL((&m.x.x)[5], 22.f);
	BOOST_CHECK_EQUAL((&m.x.x)[6], 32.f);
	BOOST_CHECK_EQUAL((&m.x.x)[7], 42.f);
	BOOST_CHECK_EQUAL((&m.x.x)[8], 13.f);
	BOOST_CHECK_EQUAL((&m.x.x)[9], 23.f);
	BOOST_CHECK_EQUAL((&m.x.x)[10], 33.f);
	BOOST_CHECK_EQUAL((&m.x.x)[11], 43.f);
	BOOST_CHECK_EQUAL((&m.x.x)[12], 14.f);
	BOOST_CHECK_EQUAL((&m.x.x)[13], 24.f);
	BOOST_CHECK_EQUAL((&m.x.x)[14], 34.f);
	BOOST_CHECK_EQUAL((&m.x.x)[15], 44.f);
}

BOOST_AUTO_TEST_CASE(matrix4_camera)
{
	const auto actual = Matrix4::camera({1, 2, 3}, {30, 45, 60});
	const auto expected = Matrix4::rotation(60, {0, -1, 0}) * Matrix4::rotation(45, {-1, 0, 0}) * Matrix4::rotation(30, {0, 0, 1}) * Matrix4::translation({-1, -2, -3});
	BOOST_CHECK_CLOSE(actual.x.x, expected.x.x, 3e-5);
	BOOST_CHECK_CLOSE(actual.x.y, expected.x.y, 3e-5);
	BOOST_CHECK_CLOSE(actual.x.z, expected.x.z, 3e-5);
	BOOST_CHECK_CLOSE(actual.x.w, expected.x.w, 3e-5);
	BOOST_CHECK_CLOSE(actual.y.x, expected.y.x, 3e-5);
	BOOST_CHECK_CLOSE(actual.y.y, expected.y.y, 3e-5);
	BOOST_CHECK_CLOSE(actual.y.z, expected.y.z, 3e-5);
	BOOST_CHECK_CLOSE(actual.y.w, expected.y.w, 3e-5);
	BOOST_CHECK_CLOSE(actual.z.x, expected.z.x, 3e-5);
	BOOST_CHECK_CLOSE(actual.z.y, expected.z.y, 3e-5);
	BOOST_CHECK_CLOSE(actual.z.z, expected.z.z, 3e-5);
	BOOST_CHECK_CLOSE(actual.z.w, expected.z.w, 3e-5);
	BOOST_CHECK_CLOSE(actual.t.x, expected.t.x, 3e-5);
	BOOST_CHECK_CLOSE(actual.t.y, expected.t.y, 3e-5);
	BOOST_CHECK_CLOSE(actual.t.z, expected.t.z, 3e-5);
	BOOST_CHECK_CLOSE(actual.t.w, expected.t.w, 3e-5);
}

BOOST_AUTO_TEST_CASE(matrix4_det)
{
	// Determinant "can be viewed as the scaling factor of the transformation described by the matrix" (c) Wikipedia.
	BOOST_CHECK_EQUAL(det(Matrix4::identity()), 1.f);
	BOOST_CHECK_EQUAL(det(Matrix4::scaling(2)), 2.f * 2.f * 2.f);
	BOOST_CHECK_EQUAL(det(Matrix4::translation({2, 3, 5})), 1.f);
	BOOST_CHECK_CLOSE(det(Matrix4::rotation(37, {2, 3, 5})), 1.f, 2e-5);
	BOOST_CHECK_CLOSE(det(Matrix4::scaling(4) * Matrix4::rotation(37, {2, 3, 5}) * Matrix4::scaling(2) * Matrix4::translation({2, 3, 5})), 4.f * 4.f * 4.f * 2.f * 2.f * 2.f, 2e-5);
}

BOOST_AUTO_TEST_CASE(matrix4_euler)
{
	const Matrix4 actual{Yttrium::Euler{30, 45, 60}};
	const auto expected = Matrix4::rotation(30, {0, 0, -1}) * Matrix4::rotation(45, {1, 0, 0}) * Matrix4::rotation(60, {0, 1, 0});
	BOOST_CHECK_CLOSE(actual.x.x, expected.x.x, 3e-5);
	BOOST_CHECK_CLOSE(actual.x.y, expected.x.y, 3e-5);
	BOOST_CHECK_CLOSE(actual.x.z, expected.x.z, 3e-5);
	BOOST_CHECK_CLOSE(actual.x.w, expected.x.w, 3e-5);
	BOOST_CHECK_CLOSE(actual.y.x, expected.y.x, 3e-5);
	BOOST_CHECK_CLOSE(actual.y.y, expected.y.y, 3e-5);
	BOOST_CHECK_CLOSE(actual.y.z, expected.y.z, 3e-5);
	BOOST_CHECK_CLOSE(actual.y.w, expected.y.w, 3e-5);
	BOOST_CHECK_CLOSE(actual.z.x, expected.z.x, 3e-5);
	BOOST_CHECK_CLOSE(actual.z.y, expected.z.y, 3e-5);
	BOOST_CHECK_CLOSE(actual.z.z, expected.z.z, 3e-5);
	BOOST_CHECK_CLOSE(actual.z.w, expected.z.w, 3e-5);
	BOOST_CHECK_CLOSE(actual.t.x, expected.t.x, 3e-5);
	BOOST_CHECK_CLOSE(actual.t.y, expected.t.y, 3e-5);
	BOOST_CHECK_CLOSE(actual.t.z, expected.t.z, 3e-5);
	BOOST_CHECK_CLOSE(actual.t.w, expected.t.w, 3e-5);
}

BOOST_AUTO_TEST_CASE(matrix4_inverse)
{
	BOOST_CHECK_EQUAL(inverse(Matrix4::identity()), Matrix4::identity());
	BOOST_CHECK_EQUAL(inverse(Matrix4::translation({2, 3, 5})), Matrix4::translation({-2, -3, -5}));
	BOOST_CHECK_EQUAL(inverse(Matrix4::scaling(4)), Matrix4::scaling(0.25));
	{
		const auto actual = inverse(Matrix4::rotation(37, {2, 3, 5}));
		const auto expected = Matrix4::rotation(-37, {2, 3, 5});
		BOOST_CHECK_CLOSE(actual.x.x, expected.x.x, 2e-5);
		BOOST_CHECK_CLOSE(actual.x.y, expected.x.y, 2e-5);
		BOOST_CHECK_CLOSE(actual.x.z, expected.x.z, 2e-5);
		BOOST_CHECK_CLOSE(actual.x.w, expected.x.w, 2e-5);
		BOOST_CHECK_CLOSE(actual.y.x, expected.y.x, 2e-5);
		BOOST_CHECK_CLOSE(actual.y.y, expected.y.y, 2e-5);
		BOOST_CHECK_CLOSE(actual.y.z, expected.y.z, 2e-5);
		BOOST_CHECK_CLOSE(actual.y.w, expected.y.w, 2e-5);
		BOOST_CHECK_CLOSE(actual.z.x, expected.z.x, 2e-5);
		BOOST_CHECK_CLOSE(actual.z.y, expected.z.y, 2e-5);
		BOOST_CHECK_CLOSE(actual.z.z, expected.z.z, 2e-5);
		BOOST_CHECK_CLOSE(actual.z.w, expected.z.w, 2e-5);
		BOOST_CHECK_CLOSE(actual.t.x, expected.t.x, 2e-5);
		BOOST_CHECK_CLOSE(actual.t.y, expected.t.y, 2e-5);
		BOOST_CHECK_CLOSE(actual.t.z, expected.t.z, 2e-5);
		BOOST_CHECK_CLOSE(actual.t.w, expected.t.w, 2e-5);
	}
	{
		const auto actual = inverse(Matrix4::scaling(4) * Matrix4::rotation(37, {2, 3, 5}) * Matrix4::scaling(2) * Matrix4::translation({2, 3, 5}));
		const auto expected = Matrix4::translation({-2, -3, -5}) * Matrix4::scaling(0.5) * Matrix4::rotation(-37, {2, 3, 5}) * Matrix4::scaling(0.25);
		BOOST_CHECK_CLOSE(actual.x.x, expected.x.x, 2e-5);
		BOOST_CHECK_CLOSE(actual.x.y, expected.x.y, 2e-5);
		BOOST_CHECK_CLOSE(actual.x.z, expected.x.z, 2e-5);
		BOOST_CHECK_CLOSE(actual.x.w, expected.x.w, 2e-5);
		BOOST_CHECK_CLOSE(actual.y.x, expected.y.x, 2e-5);
		BOOST_CHECK_CLOSE(actual.y.y, expected.y.y, 2e-5);
		BOOST_CHECK_CLOSE(actual.y.z, expected.y.z, 2e-5);
		BOOST_CHECK_CLOSE(actual.y.w, expected.y.w, 2e-5);
		BOOST_CHECK_CLOSE(actual.z.x, expected.z.x, 2e-5);
		BOOST_CHECK_CLOSE(actual.z.y, expected.z.y, 2e-5);
		BOOST_CHECK_CLOSE(actual.z.z, expected.z.z, 2e-5);
		BOOST_CHECK_CLOSE(actual.z.w, expected.z.w, 2e-5);
		BOOST_CHECK_CLOSE(actual.t.x, expected.t.x, 2e-5);
		BOOST_CHECK_CLOSE(actual.t.y, expected.t.y, 2e-5);
		BOOST_CHECK_CLOSE(actual.t.z, expected.t.z, 2e-5);
		BOOST_CHECK_CLOSE(actual.t.w, expected.t.w, 2e-5);
	}
}

BOOST_AUTO_TEST_CASE(matrix4_perspective)
{
	using Yttrium::Vector3;

	const auto m = Matrix4::perspective({1, 1}, 90, 1, 2);
	{
		const auto v = m * Vector3{-1, -1, -1};
		BOOST_CHECK_CLOSE(v.x, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, -1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{-2, -2, -2};
		BOOST_CHECK_CLOSE(v.x, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, 1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{-1, 1, -1};
		BOOST_CHECK_CLOSE(v.x, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, -1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{-2, 2, -2};
		BOOST_CHECK_CLOSE(v.x, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, 1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{1, -1, -1};
		BOOST_CHECK_CLOSE(v.x, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, -1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{2, -2, -2};
		BOOST_CHECK_CLOSE(v.x, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, 1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{1, 1, -1};
		BOOST_CHECK_CLOSE(v.x, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, -1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{2, 2, -2};
		BOOST_CHECK_CLOSE(v.x, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, 1.f, 2e-5);
	}
}

BOOST_AUTO_TEST_CASE(matrix4_projection_2d)
{
	using Yttrium::Vector3;

	const auto m = Matrix4::projection_2d({640, 480}, -.25, .75);
	{
		const auto v = m * Vector3{0, 0, -.25};
		BOOST_CHECK_CLOSE(v.x, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, 1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{0, 0, .75};
		BOOST_CHECK_CLOSE(v.x, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, -1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{640, 0, -.25};
		BOOST_CHECK_CLOSE(v.x, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, 1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{640, 0, .75};
		BOOST_CHECK_CLOSE(v.x, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, -1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{0, 480, -.25};
		BOOST_CHECK_CLOSE(v.x, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, 1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{0, 480, .75};
		BOOST_CHECK_CLOSE(v.x, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, -1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{640, 480, -.25};
		BOOST_CHECK_CLOSE(v.x, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, 1.f, 2e-5);
	}
	{
		const auto v = m * Vector3{640, 480, .75};
		BOOST_CHECK_CLOSE(v.x, 1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.y, -1.f, 2e-5);
		BOOST_CHECK_CLOSE(v.z, -1.f, 2e-5);
	}
}
