#include <yttrium/math/matrix.h>

#include <yttrium/math/euler.h>
#include <yttrium/math/size.h>
#include <yttrium/math/vector3.h>
#include <yttrium/math/vector4.h>

#include <cassert>
#include <cmath>

namespace Yttrium
{
	const Matrix4 Matrix4::Identity
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

	// cppcheck-suppress uninitMemberVar
	Matrix4::Matrix4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) noexcept
		: _00{m00}, _10{m10}, _20{m20}, _30{m30}, _01{m01}, _11{m11}, _21{m21}, _31{m31}, _02{m02}, _12{m12}, _22{m22}, _32{m32}, _03{m03}, _13{m13}, _23{m23}, _33{m33}
	{
	}

	Matrix4::Matrix4(const Euler& rotation) noexcept
	{
		*this = Matrix4::rotation(rotation.yaw, {0, 0, -1})
			* Matrix4::rotation(rotation.pitch, {1, 0, 0})
			* Matrix4::rotation(rotation.roll, {0, 1, 0});
	}

	float Matrix4::determinant() const noexcept
	{
		// Rows 2 and 3.
		const auto det01 = _20 * _31 - _30 * _21;
		const auto det02 = _20 * _32 - _30 * _22;
		const auto det03 = _20 * _33 - _30 * _23;
		const auto det12 = _21 * _32 - _31 * _22;
		const auto det13 = _21 * _33 - _31 * _23;
		const auto det23 = _22 * _33 - _32 * _23;

		// Rows 1, 2 and 3.
		const auto det123 = _11 * det23 - _12 * det13 + _13 * det12;
		const auto det023 = _10 * det23 - _12 * det03 + _13 * det02;
		const auto det013 = _10 * det13 - _11 * det03 + _13 * det01;
		const auto det012 = _10 * det12 - _11 * det02 + _12 * det01;

		return _00 * det123 - _01 * det023 + _02 * det013 - _03 * det012;
	}

	Matrix4 Matrix4::inversed() const noexcept
	{
		Matrix4 m;

		// Rows 2 and 3.
		auto det01 = _20 * _31 - _30 * _21;
		auto det02 = _20 * _32 - _30 * _22;
		auto det03 = _20 * _33 - _30 * _23;
		auto det12 = _21 * _32 - _31 * _22;
		auto det13 = _21 * _33 - _31 * _23;
		auto det23 = _22 * _33 - _32 * _23;

		// Rows 1, 2 and 3.
		const auto det123 = _11 * det23 - _12 * det13 + _13 * det12;
		const auto det023 = _10 * det23 - _12 * det03 + _13 * det02;
		const auto det013 = _10 * det13 - _11 * det03 + _13 * det01;
		const auto det012 = _10 * det12 - _11 * det02 + _12 * det01;

		const auto d = 1 / (_00 * det123 - _01 * det023 + _02 * det013 - _03 * det012);

		m._00 = d *  det123;
		m._10 = d * -det023;
		m._20 = d *  det013;
		m._30 = d * -det012;

		m._01 = d * -(_01 * det23 - _02 * det13 + _03 * det12);
		m._11 = d *  (_00 * det23 - _02 * det03 + _03 * det02);
		m._21 = d * -(_00 * det13 - _01 * det03 + _03 * det01);
		m._31 = d *  (_00 * det12 - _01 * det02 + _02 * det01);

		// Rows 1 and 3.
		det01 = _10 * _31 - _11 * _30;
		det02 = _10 * _32 - _12 * _30;
		det03 = _10 * _33 - _13 * _30;
		det12 = _11 * _32 - _12 * _31;
		det13 = _11 * _33 - _13 * _31;
		det23 = _12 * _33 - _13 * _32;

		m._02 = d *  (_01 * det23 - _02 * det13 + _03 * det12);
		m._12 = d * -(_00 * det23 - _02 * det03 + _03 * det02);
		m._22 = d *  (_00 * det13 - _01 * det03 + _03 * det01);
		m._32 = d * -(_00 * det12 - _01 * det02 + _02 * det01);

		// Rows 1 and 2.
		det01 = _21 * _10 - _20 * _11;
		det02 = _22 * _10 - _20 * _12;
		det03 = _23 * _10 - _20 * _13;
		det12 = _22 * _11 - _21 * _12;
		det13 = _23 * _11 - _21 * _13;
		det23 = _23 * _12 - _22 * _13;

		m._03 = d * -(_01 * det23 - _02 * det13 + _03 * det12);
		m._13 = d *  (_00 * det23 - _02 * det03 + _03 * det02);
		m._23 = d * -(_00 * det13 - _01 * det03 + _03 * det01);
		m._33 = d *  (_00 * det12 - _01 * det02 + _02 * det01);

		return m;
	}

	Matrix4 Matrix4::camera(const Vector4& position, const Euler& rotation) noexcept
	{
		return Matrix4::rotation(rotation.roll, {0, -1, 0})
			* Matrix4::rotation(rotation.pitch, {-1, 0, 0})
			* Matrix4::rotation(rotation.yaw, {0, 0, 1})
			* Matrix4::translation(-position);
	}

	Matrix4 Matrix4::projection_2d(const Size& size, float near, float far) noexcept
	{
		assert(size.width() > 0 && size.height() > 0 && far > near);

		constexpr auto left = 0.f;
		constexpr auto top = 0.f;

		const auto m00 = 2 / static_cast<float>(size.width());
		const auto m11 = -2 / static_cast<float>(size.height());
		const auto m22 = 2 / (near - far);
		const auto m03 = -1 - m00 * left;
		const auto m13 = 1 - m11 * top;
		const auto m23 = (near + far) / (near - far);

		return  Matrix4
		(
			m00, 0,   0,   m03,
			0,   m11, 0,   m13,
			0,   0,   m22, m23,
			0,   0,   0,   1
		);
	}

	Matrix4 Matrix4::perspective(const Size& size, float vertical_fov, float near, float far) noexcept
	{
		assert(size.width() > 0 && size.height() > 0 && vertical_fov > 0 && vertical_fov < 360 && near > 0 && far > near);

		const auto aspect = static_cast<float>(size.width()) / static_cast<float>(size.height());
		const auto fov_radians = vertical_fov / 180 * M_PI;

		const auto f = 1 / std::tan(fov_radians / 2);

		const auto m00 = f / aspect;
		const auto m11 = f;
		const auto m22 = (near + far) / (near - far);
		const auto m23 = 2 * near * far / (near - far);
		const auto m32 = -1.f;

		return Matrix4
		(
			m00, 0,   0,   0,
			0,   m11, 0,   0,
			0,   0,   m22, m23,
			0,   0,   m32, 0
		);
	}

	Matrix4 Matrix4::rotation(float angle, const Vector4& axis) noexcept
	{
		const auto& v = axis.normalized();
		const auto angle_radians = angle / 180 * M_PI;
		const auto c = std::cos(angle_radians);
		const auto s = std::sin(angle_radians);

		return Matrix4
		(
			v.x * v.x * (1 - c) + c,       v.y * v.x * (1 - c) - s * v.z, v.z * v.x * (1 - c) + s * v.y, 0,
			v.x * v.y * (1 - c) + s * v.z, v.y * v.y * (1 - c) + c,       v.z * v.y * (1 - c) - s * v.x, 0,
			v.x * v.z * (1 - c) - s * v.y, v.y * v.z * (1 - c) + s * v.x, v.z * v.z * (1 - c) + c,       0,
			0,                             0,                             0,                             1
		);
	}

	Matrix4 Matrix4::scaling(float scale) noexcept
	{
		return Matrix4
		(
			scale, 0,     0,     0,
			0,     scale, 0,     0,
			0,     0,     scale, 0,
			0,     0,     0,     1
		);
	}

	Matrix4 Matrix4::translation(const Vector4& v) noexcept
	{
		return Matrix4
		(
			1, 0, 0, v.x,
			0, 1, 0, v.y,
			0, 0, 1, v.z,
			0, 0, 0, 1
		);
	}

	Matrix4 operator*(const Matrix4& a, const Matrix4& b) noexcept
	{
		return
		{
			a(0,0) * b(0,0) + a(0,1) * b(1,0) + a(0,2) * b(2,0) + a(0,3) * b(3,0),
			a(0,0) * b(0,1) + a(0,1) * b(1,1) + a(0,2) * b(2,1) + a(0,3) * b(3,1),
			a(0,0) * b(0,2) + a(0,1) * b(1,2) + a(0,2) * b(2,2) + a(0,3) * b(3,2),
			a(0,0) * b(0,3) + a(0,1) * b(1,3) + a(0,2) * b(2,3) + a(0,3) * b(3,3),

			a(1,0) * b(0,0) + a(1,1) * b(1,0) + a(1,2) * b(2,0) + a(1,3) * b(3,0),
			a(1,0) * b(0,1) + a(1,1) * b(1,1) + a(1,2) * b(2,1) + a(1,3) * b(3,1),
			a(1,0) * b(0,2) + a(1,1) * b(1,2) + a(1,2) * b(2,2) + a(1,3) * b(3,2),
			a(1,0) * b(0,3) + a(1,1) * b(1,3) + a(1,2) * b(2,3) + a(1,3) * b(3,3),

			a(2,0) * b(0,0) + a(2,1) * b(1,0) + a(2,2) * b(2,0) + a(2,3) * b(3,0),
			a(2,0) * b(0,1) + a(2,1) * b(1,1) + a(2,2) * b(2,1) + a(2,3) * b(3,1),
			a(2,0) * b(0,2) + a(2,1) * b(1,2) + a(2,2) * b(2,2) + a(2,3) * b(3,2),
			a(2,0) * b(0,3) + a(2,1) * b(1,3) + a(2,2) * b(2,3) + a(2,3) * b(3,3),

			a(3,0) * b(0,0) + a(3,1) * b(1,0) + a(3,2) * b(2,0) + a(3,3) * b(3,0),
			a(3,0) * b(0,1) + a(3,1) * b(1,1) + a(3,2) * b(2,1) + a(3,3) * b(3,1),
			a(3,0) * b(0,2) + a(3,1) * b(1,2) + a(3,2) * b(2,2) + a(3,3) * b(3,2),
			a(3,0) * b(0,3) + a(3,1) * b(1,3) + a(3,2) * b(2,3) + a(3,3) * b(3,3),
		};
	}

	Vector3 operator*(const Matrix4& m, const Vector3& v) noexcept
	{
		return
		{
			m(0,0) * v.x + m(0,1) * v.y + m(0,2) * v.z + m(0,3),
			m(1,0) * v.x + m(1,1) * v.y + m(1,2) * v.z + m(1,3),
			m(2,0) * v.x + m(2,1) * v.y + m(2,2) * v.z + m(2,3),
		};
	}

	Vector4 operator*(const Matrix4& m, const Vector4& v) noexcept
	{
		return
		{
			m(0,0) * v.x + m(0,1) * v.y + m(0,2) * v.z + m(0,3) * v.w,
			m(1,0) * v.x + m(1,1) * v.y + m(1,2) * v.z + m(1,3) * v.w,
			m(2,0) * v.x + m(2,1) * v.y + m(2,2) * v.z + m(2,3) * v.w,
			m(3,0) * v.x + m(3,1) * v.y + m(3,2) * v.z + m(3,3) * v.w,
		};
	}
}
