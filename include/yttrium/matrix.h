/// \file
/// \brief

#ifndef __Y_MATRIX_H
#define __Y_MATRIX_H

#include <yttrium/global.h>

namespace Yttrium
{
	class Size;

	///
	class Y_API Matrix4f
	{
	public:

		///
		Matrix4f();

		///
		Matrix4f(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);

		///
		const float *data() const { return _data[0]; }

		///
		static Matrix4f projection_2d(const Size& size, float near = -1.f, float far = 1.f);

		///
		static Matrix4f perspective(float aspect, float vertical_fov, float near, float far);

	private:

		float _data[4][4];
	};
}

#endif
