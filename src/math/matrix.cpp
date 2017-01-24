#define _USE_MATH_DEFINES

#include <yttrium/math/matrix.h>

#include <yttrium/math/euler.h>
#include <yttrium/math/size.h>
#include <yttrium/math/vector.h>

#include <cassert>
#include <cmath>

namespace Yttrium
{
	Matrix4::Matrix4()
		: _data{
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}}
	{
	}

	Matrix4::Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
		: _data{
			{m00, m10, m20, m30},
			{m01, m11, m21, m31},
			{m02, m12, m22, m32},
			{m03, m13, m23, m33}}
	{
	}

	Matrix4::Matrix4(const Euler& rotation)
	{
		*this = Matrix4::rotation(rotation.yaw, {0, 0, -1})
			* Matrix4::rotation(rotation.pitch, {1, 0, 0})
			* Matrix4::rotation(rotation.roll, {0, 1, 0});
	}

	Matrix4 Matrix4::camera(const Vector4& position, const Euler& rotation)
	{
		return Matrix4::rotation(rotation.roll, {0, -1, 0})
			* Matrix4::rotation(rotation.pitch, {-1, 0, 0})
			* Matrix4::rotation(rotation.yaw, {0, 0, 1})
			* Matrix4::translation(-position);
	}

	Matrix4 Matrix4::projection_2d(const Size& size, float near, float far)
	{
		assert(size.width() > 0 && size.height() > 0 && far > near);

		const auto left = 0.f;
		const auto top = 0.f;

		const auto m00 = 2.f / size.width();
		const auto m11 = -2.f / size.height();
		const auto m22 = 2 / (near - far);
		const auto m03 = -1 - m00 * left;
		const auto m13 = 1 - m11 * top;
		const auto m23 = (near + far) / (near - far);

		return Matrix4(m00, 0, 0, m03, 0, m11, 0, m13, 0, 0, m22, m23, 0, 0, 0, 1);
	}

	Matrix4 Matrix4::perspective(const Size& size, float vertical_fov, float near, float far)
	{
		assert(size.width() > 0 && size.height() > 0 && vertical_fov > 0 && vertical_fov < 360 && near > 0 && far > near);

		const float aspect = static_cast<float>(size.width()) / size.height();
		const float fov_radians = vertical_fov / 180 * M_PI;

		const auto f = 1 / ::tan(fov_radians / 2);

		const auto m00 = f / aspect;
		const auto m11 = f;
		const auto m22 = (near + far) / (near - far);
		const auto m23 = 2 * near * far / (near - far);
		const auto m32 = -1.f;

		return Matrix4(m00, 0, 0, 0, 0, m11, 0, 0, 0, 0, m22, m23, 0, 0, m32, 0);
	}

	Matrix4 Matrix4::rotation(float angle, const Vector4& axis)
	{
		const auto& v = axis.normalized();
		const float angle_radians = angle / 180 * M_PI;
		const auto c = ::cos(angle_radians);
		const auto s = ::sin(angle_radians);

		return Matrix4(
			v.x * v.x * (1 - c) + c,       v.y * v.x * (1 - c) - s * v.z, v.z * v.x * (1 - c) + s * v.y, 0,
			v.x * v.y * (1 - c) + s * v.z, v.y * v.y * (1 - c) + c,       v.z * v.y * (1 - c) - s * v.x, 0,
			v.x * v.z * (1 - c) - s * v.y, v.y * v.z * (1 - c) + s * v.x, v.z * v.z * (1 - c) + c,       0,
			0,                             0,                             0,                             1);
	}

	Matrix4 Matrix4::scaling(float scale)
	{
		return Matrix4
		(
			scale, 0,     0,     0,
			0,     scale, 0,     0,
			0,     0,     scale, 0,
			0,     0,     0,     1
		);
	}

	Matrix4 Matrix4::translation(const Vector4& point)
	{
		return Matrix4
		(
			1, 0, 0, point.x,
			0, 1, 0, point.y,
			0, 0, 1, point.z,
			0, 0, 0, 1
		);
	}

	Matrix4 operator*(const Matrix4& a, const Matrix4& b)
	{
		return Matrix4
		(
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
			a(3,0) * b(0,3) + a(3,1) * b(1,3) + a(3,2) * b(2,3) + a(3,3) * b(3,3)
		);
	}

	Vector4 operator*(const Matrix4& a, const Vector4& b)
	{
		return
		{
			a(0,0) * b.x + a(0,1) * b.y + a(0,2) * b.z + a(0,3) * b.w,
			a(1,0) * b.x + a(1,1) * b.y + a(1,2) * b.z + a(1,3) * b.w,
			a(2,0) * b.x + a(2,1) * b.y + a(2,2) * b.z + a(2,3) * b.w,
			a(3,0) * b.x + a(3,1) * b.y + a(3,2) * b.z + a(3,3) * b.w,
		};
	}
}
