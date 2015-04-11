#include <yttrium/matrix.h>

#include <yttrium/size.h>

#include <cassert>
#include <cmath>

namespace Yttrium
{
	Matrix4f::Matrix4f()
		: _data{
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}}
	{
	}

	Matrix4f::Matrix4f(
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

	Matrix4f Matrix4f::projection_2d(const Size& size, float near, float far)
	{
		assert(size.width > 0 && size.height > 0 && far > near);

		const auto left = 0.f;
		const auto top = 0.f;

		const auto m00 = 2.f / size.width;
		const auto m11 = -2.f / size.height;
		const auto m22 = 2 / (near - far);
		const auto m03 = -1 - m00 * left;
		const auto m13 = 1 - m11 * top;
		const auto m23 = (near + far) / (near - far);

		return Matrix4f(m00, 0, 0, m03, 0, m11, 0, m13, 0, 0, m22, m23, 0, 0, 0, 1);
	}

	Matrix4f Matrix4f::perspective_3d(float aspect, float vertical_fov, float near, float far)
	{
		assert(aspect > 0 && vertical_fov > 0 && vertical_fov < 360 && near > 0 && far > near);

		const auto fov_radians = vertical_fov / 180 * M_PI;

		const auto f = 1 / ::tan(fov_radians / 2);

		const auto m00 = f / aspect;
		const auto m11 = f;
		const auto m22 = (near + far) / (near - far);
		const auto m23 = 2 * near * far / (near - far);
		const auto m32 = -1.f;

		return Matrix4f(m00, 0, 0, 0, 0, m11, 0, 0, 0, 0, m22, m23, 0, 0, m32, 0);
	}
}
