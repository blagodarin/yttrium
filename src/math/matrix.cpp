#include <yttrium/math/matrix.h>

#include <yttrium/math/euler.h>

#include <cassert>

namespace Yttrium
{
	Matrix4::Matrix4(const Euler& euler) noexcept
		: Matrix4{Matrix4::rotation(euler._yaw, {0, 0, -1}) * Matrix4::rotation(euler._pitch, {1, 0, 0}) * Matrix4::rotation(euler._roll, {0, 1, 0})}
	{
	}

	Matrix4 Matrix4::camera(const Vector3& position, const Euler& orientation) noexcept
	{
		return Matrix4::rotation(orientation._roll, {0, -1, 0}) * Matrix4::rotation(orientation._pitch, {-1, 0, 0}) * Matrix4::rotation(orientation._yaw, {0, 0, 1}) * Matrix4::translation(-position);
	}

	Matrix4 Matrix4::perspective(const Size& size, float vertical_fov, float near, float far) noexcept
	{
		assert(size._width > 0 && size._height > 0 && vertical_fov > 0 && vertical_fov < 360 && near > 0 && far > near);

		const auto aspect = static_cast<float>(size._width) / static_cast<float>(size._height);
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

	Matrix4 Matrix4::rotation(float angle, const Vector3& axis) noexcept
	{
		const auto v = normalize(axis);
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
}
