#include "iostream.h"

#include <boost/test/unit_test.hpp>

namespace Yttrium // Anonymous namespace breaks compilation.
{
	constexpr bool operator==(const Vector4& a, const Vector4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
	constexpr bool operator==(const Matrix4& a, const Matrix4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.t == b.t; }
}

using Yttrium::Matrix4;

BOOST_AUTO_TEST_CASE(test_matrix4_layout)
{
	const Matrix4 m
	{
		00, 01, 02, 03,
		10, 11, 12, 13,
		20, 21, 22, 23,
		30, 31, 32, 33,
	};

	BOOST_CHECK_EQUAL(m.x.x, 00);
	BOOST_CHECK_EQUAL(m.y.x, 01);
	BOOST_CHECK_EQUAL(m.z.x, 02);
	BOOST_CHECK_EQUAL(m.t.x, 03);
	BOOST_CHECK_EQUAL(m.x.y, 10);
	BOOST_CHECK_EQUAL(m.y.y, 11);
	BOOST_CHECK_EQUAL(m.z.y, 12);
	BOOST_CHECK_EQUAL(m.t.y, 13);
	BOOST_CHECK_EQUAL(m.x.z, 20);
	BOOST_CHECK_EQUAL(m.y.z, 21);
	BOOST_CHECK_EQUAL(m.z.z, 22);
	BOOST_CHECK_EQUAL(m.t.z, 23);
	BOOST_CHECK_EQUAL(m.x.w, 30);
	BOOST_CHECK_EQUAL(m.y.w, 31);
	BOOST_CHECK_EQUAL(m.z.w, 32);
	BOOST_CHECK_EQUAL(m.t.w, 33);

	BOOST_CHECK_EQUAL(m.data()[0], 00);
	BOOST_CHECK_EQUAL(m.data()[1], 10);
	BOOST_CHECK_EQUAL(m.data()[2], 20);
	BOOST_CHECK_EQUAL(m.data()[3], 30);
	BOOST_CHECK_EQUAL(m.data()[4], 01);
	BOOST_CHECK_EQUAL(m.data()[5], 11);
	BOOST_CHECK_EQUAL(m.data()[6], 21);
	BOOST_CHECK_EQUAL(m.data()[7], 31);
	BOOST_CHECK_EQUAL(m.data()[8], 02);
	BOOST_CHECK_EQUAL(m.data()[9], 12);
	BOOST_CHECK_EQUAL(m.data()[10], 22);
	BOOST_CHECK_EQUAL(m.data()[11], 32);
	BOOST_CHECK_EQUAL(m.data()[12], 03);
	BOOST_CHECK_EQUAL(m.data()[13], 13);
	BOOST_CHECK_EQUAL(m.data()[14], 23);
	BOOST_CHECK_EQUAL(m.data()[15], 33);
}

BOOST_AUTO_TEST_CASE(test_matrix4_det)
{
	using Yttrium::Vector4;

	// Determinant "can be viewed as the scaling factor of the transformation described by the matrix" (c) Wikipedia.
	BOOST_CHECK_EQUAL(det(Matrix4::identity()), 1);
	BOOST_CHECK_EQUAL(det(Matrix4::scaling(2)), 2 * 2 * 2);
	BOOST_CHECK_EQUAL(det(Matrix4::translation({2, 3, 5})), 1);
	BOOST_CHECK_CLOSE(det(Matrix4::rotation(37, {2, 3, 5})), 1, 2e-5);
	BOOST_CHECK_CLOSE(det(Matrix4::scaling(4) * Matrix4::rotation(37, {2, 3, 5}) * Matrix4::scaling(2) * Matrix4::translation({2, 3, 5})), 4 * 4 * 4 * 2 * 2 * 2, 2e-5);
}

BOOST_AUTO_TEST_CASE(test_matrix4_inverse)
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
