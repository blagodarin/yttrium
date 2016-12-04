/// \file
/// \brief

#ifndef _include_yttrium_math_matrix_h_
#define _include_yttrium_math_matrix_h_

#include <yttrium/api.h>

namespace Yttrium
{
	class Euler;
	class Size;
	class Vector4;

	///
	class Y_API Matrix4
	{
	public:

		///
		Matrix4();

		///
		Matrix4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);

		/// Constructs a rotation matrix from Euler angles.
		explicit Matrix4(const Euler&);

		///
		const float* data() const { return _data[0]; }

		///
		float operator()(int row, int column) const { return _data[column][row]; }

		///
		static Matrix4 camera(const Vector4& position, const Euler& rotation);

		///
		static Matrix4 projection_2d(const Size& size, float near = -1.f, float far = 1.f);

		///
		static Matrix4 perspective(const Size& size, float vertical_fov, float near, float far);

		///
		static Matrix4 rotation(float angle, const Vector4& axis);

		///
		static Matrix4 scaling(float scale);

		///
		static Matrix4 translation(const Vector4& point);

	private:

		float _data[4][4];
	};

	Y_API Matrix4 operator*(const Matrix4&, const Matrix4&);
	Y_API Vector4 operator*(const Matrix4&, const Vector4&);
}

#endif
