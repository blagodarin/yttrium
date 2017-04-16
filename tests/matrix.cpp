#include "iostream.h"

#include <yttrium/math/vector4.h>

#include <boost/test/unit_test.hpp>

namespace Yttrium // Anonymous namespace breaks compilation.
{
	inline bool operator==(const Matrix4& a, const Matrix4& b)
	{
		return a(0,0) == b(0,0) && a(0,1) == b(0,1) && a(0,2) == b(0,2) && a(0,3) == b(0,3)
			&& a(1,0) == b(1,0) && a(1,1) == b(1,1) && a(1,2) == b(1,2) && a(1,3) == b(1,3)
			&& a(2,0) == b(2,0) && a(2,1) == b(2,1) && a(2,2) == b(2,2) && a(2,3) == b(2,3)
			&& a(3,0) == b(3,0) && a(3,1) == b(3,1) && a(3,2) == b(3,2) && a(3,3) == b(3,3);
	}
}

using Yttrium::Matrix4;

BOOST_AUTO_TEST_CASE(test_matrix4)
{
	Matrix4 m{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	BOOST_CHECK_EQUAL(m(0, 0), 0);
	BOOST_CHECK_EQUAL(m(0, 1), 1);
	BOOST_CHECK_EQUAL(m(0, 2), 2);
	BOOST_CHECK_EQUAL(m(0, 3), 3);
	BOOST_CHECK_EQUAL(m(1, 0), 4);
	BOOST_CHECK_EQUAL(m(1, 1), 5);
	BOOST_CHECK_EQUAL(m(1, 2), 6);
	BOOST_CHECK_EQUAL(m(1, 3), 7);
	BOOST_CHECK_EQUAL(m(2, 0), 8);
	BOOST_CHECK_EQUAL(m(2, 1), 9);
	BOOST_CHECK_EQUAL(m(2, 2), 10);
	BOOST_CHECK_EQUAL(m(2, 3), 11);
	BOOST_CHECK_EQUAL(m(3, 0), 12);
	BOOST_CHECK_EQUAL(m(3, 1), 13);
	BOOST_CHECK_EQUAL(m(3, 2), 14);
	BOOST_CHECK_EQUAL(m(3, 3), 15);

	BOOST_CHECK_EQUAL(m.data()[0], 0);
	BOOST_CHECK_EQUAL(m.data()[1], 4);
	BOOST_CHECK_EQUAL(m.data()[2], 8);
	BOOST_CHECK_EQUAL(m.data()[3], 12);
	BOOST_CHECK_EQUAL(m.data()[4], 1);
	BOOST_CHECK_EQUAL(m.data()[5], 5);
	BOOST_CHECK_EQUAL(m.data()[6], 9);
	BOOST_CHECK_EQUAL(m.data()[7], 13);
	BOOST_CHECK_EQUAL(m.data()[8], 2);
	BOOST_CHECK_EQUAL(m.data()[9], 6);
	BOOST_CHECK_EQUAL(m.data()[10], 10);
	BOOST_CHECK_EQUAL(m.data()[11], 14);
	BOOST_CHECK_EQUAL(m.data()[12], 3);
	BOOST_CHECK_EQUAL(m.data()[13], 7);
	BOOST_CHECK_EQUAL(m.data()[14], 11);
	BOOST_CHECK_EQUAL(m.data()[15], 15);
}

BOOST_AUTO_TEST_CASE(test_matrix4_det)
{
	using Yttrium::Vector4;

	// Determinant "can be viewed as the scaling factor of the transformation described by the matrix" (c) Wikipedia.
	BOOST_CHECK_EQUAL(Matrix4::Identity.determinant(), 1);
	BOOST_CHECK_EQUAL(Matrix4::scaling(2).determinant(), 2 * 2 * 2);
	BOOST_CHECK_EQUAL(Matrix4::translation({2, 3, 5}).determinant(), 1);
	BOOST_CHECK_CLOSE(Matrix4::rotation(37, {2, 3, 5}).determinant(), 1, 2e-5);
	BOOST_CHECK_CLOSE((Matrix4::scaling(4) * Matrix4::rotation(37, {2, 3, 5}) * Matrix4::scaling(2) * Matrix4::translation({2, 3, 5})).determinant(), 4 * 4 * 4 * 2 * 2 * 2, 2e-5);
}

BOOST_AUTO_TEST_CASE(test_matrix4_inverse)
{
	BOOST_CHECK_EQUAL(Matrix4::Identity.inversed(), Matrix4::Identity);
	BOOST_CHECK_EQUAL(Matrix4::translation({2, 3, 5}).inversed(), Matrix4::translation({-2, -3, -5}));
	BOOST_CHECK_EQUAL(Matrix4::scaling(4).inversed(), Matrix4::scaling(0.25));
	{
		const auto actual = Matrix4::rotation(37, {2, 3, 5}).inversed();
		const auto expected = Matrix4::rotation(-37, {2, 3, 5});
		BOOST_CHECK_CLOSE(actual(0,0), expected(0,0), 2e-5);
		BOOST_CHECK_CLOSE(actual(0,1), expected(0,1), 2e-5);
		BOOST_CHECK_CLOSE(actual(0,2), expected(0,2), 2e-5);
		BOOST_CHECK_CLOSE(actual(0,3), expected(0,3), 2e-5);
		BOOST_CHECK_CLOSE(actual(1,0), expected(1,0), 2e-5);
		BOOST_CHECK_CLOSE(actual(1,1), expected(1,1), 2e-5);
		BOOST_CHECK_CLOSE(actual(1,2), expected(1,2), 2e-5);
		BOOST_CHECK_CLOSE(actual(1,3), expected(1,3), 2e-5);
		BOOST_CHECK_CLOSE(actual(2,0), expected(2,0), 2e-5);
		BOOST_CHECK_CLOSE(actual(2,1), expected(2,1), 2e-5);
		BOOST_CHECK_CLOSE(actual(2,2), expected(2,2), 2e-5);
		BOOST_CHECK_CLOSE(actual(2,3), expected(2,3), 2e-5);
		BOOST_CHECK_CLOSE(actual(3,0), expected(3,0), 2e-5);
		BOOST_CHECK_CLOSE(actual(3,1), expected(3,1), 2e-5);
		BOOST_CHECK_CLOSE(actual(3,2), expected(3,2), 2e-5);
		BOOST_CHECK_CLOSE(actual(3,3), expected(3,3), 2e-5);
	}
	{
		const auto actual = (Matrix4::scaling(4) * Matrix4::rotation(37, {2, 3, 5}) * Matrix4::scaling(2) * Matrix4::translation({2, 3, 5})).inversed();
		const auto expected = Matrix4::translation({-2, -3, -5}) * Matrix4::scaling(0.5) * Matrix4::rotation(-37, {2, 3, 5}) * Matrix4::scaling(0.25);
		BOOST_CHECK_CLOSE(actual(0,0), expected(0,0), 2e-5);
		BOOST_CHECK_CLOSE(actual(0,1), expected(0,1), 2e-5);
		BOOST_CHECK_CLOSE(actual(0,2), expected(0,2), 2e-5);
		BOOST_CHECK_CLOSE(actual(0,3), expected(0,3), 2e-5);
		BOOST_CHECK_CLOSE(actual(1,0), expected(1,0), 2e-5);
		BOOST_CHECK_CLOSE(actual(1,1), expected(1,1), 2e-5);
		BOOST_CHECK_CLOSE(actual(1,2), expected(1,2), 2e-5);
		BOOST_CHECK_CLOSE(actual(1,3), expected(1,3), 2e-5);
		BOOST_CHECK_CLOSE(actual(2,0), expected(2,0), 2e-5);
		BOOST_CHECK_CLOSE(actual(2,1), expected(2,1), 2e-5);
		BOOST_CHECK_CLOSE(actual(2,2), expected(2,2), 2e-5);
		BOOST_CHECK_CLOSE(actual(2,3), expected(2,3), 2e-5);
		BOOST_CHECK_CLOSE(actual(3,0), expected(3,0), 2e-5);
		BOOST_CHECK_CLOSE(actual(3,1), expected(3,1), 2e-5);
		BOOST_CHECK_CLOSE(actual(3,2), expected(3,2), 2e-5);
		BOOST_CHECK_CLOSE(actual(3,3), expected(3,3), 2e-5);
	}
}
