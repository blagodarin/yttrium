#include <yttrium/matrix.h>

#include <yttrium/assert.h>

namespace Yttrium
{

Matrix4f Matrix4f::projection_2d(float left, float top, float width, float height, float near, float far)
{
	Y_ASSERT(width > 0.f && height > 0.f && far > near);

	float inverse_depth = near - far;

	float m00 = 2 / width;
	float m11 = -2 / height;
	float m22 = 2 / inverse_depth;

	float m03 = -1 - m00 * left;
	float m13 = 1 - m11 * top;
	float m23 = (near + far) / inverse_depth;

	return Matrix4f(m00, 0, 0, m03, 0, m11, 0, m13, 0, 0, m22, m23, 0, 0, 0, 1);
}

} // namespace Yttrium
