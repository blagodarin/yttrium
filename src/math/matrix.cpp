#include <yttrium/matrix.h>

#include <cassert>

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

	Matrix4f Matrix4f::projection_2d(float left, float top, float width, float height, float near, float far)
	{
		assert(width > 0.f && height > 0.f && far > near);

		const float inverse_depth = near - far;

		const float m00 = 2 / width;
		const float m11 = -2 / height;
		const float m22 = 2 / inverse_depth;

		const float m03 = -1 - m00 * left;
		const float m13 = 1 - m11 * top;
		const float m23 = (near + far) / inverse_depth;

		return Matrix4f(m00, 0, 0, m03, 0, m11, 0, m13, 0, 0, m22, m23, 0, 0, 0, 1);
	}
}
