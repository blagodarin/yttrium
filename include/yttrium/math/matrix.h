/// \file
/// \brief

#ifndef _include_yttrium_math_matrix_h_
#define _include_yttrium_math_matrix_h_

#include <yttrium/api.h>

namespace Yttrium
{
	class Euler;
	class Size;
	class Vector3;
	class Vector4;

	///
	class Y_API Matrix4
	{
	public:
		///
		static const Matrix4 Identity;

		///
		Matrix4() = default;

		///
		Matrix4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33) noexcept;

		/// Constructs a rotation matrix from Euler angles.
		explicit Matrix4(const Euler&) noexcept;

		///
		const float* data() const noexcept { return &_00; }

		///
		float determinant() const noexcept;

		///
		Matrix4 inversed() const noexcept;

		///
		float operator()(int row, int column) const noexcept { return data()[column * 4 + row]; }

		///
		static Matrix4 camera(const Vector4& position, const Euler& rotation) noexcept;

		///
		static Matrix4 projection_2d(const Size& size, float near = -1.f, float far = 1.f) noexcept;

		///
		static Matrix4 perspective(const Size& size, float vertical_fov, float near, float far) noexcept;

		///
		static Matrix4 rotation(float angle, const Vector4& axis) noexcept;

		///
		static Matrix4 scaling(float scale) noexcept;

		///
		static Matrix4 translation(const Vector4& point) noexcept;

	private:
		float _00, _10, _20, _30;
		float _01, _11, _21, _31;
		float _02, _12, _22, _32;
		float _03, _13, _23, _33;
	};

	Y_API Matrix4 operator*(const Matrix4&, const Matrix4&) noexcept;
	Y_API Vector3 operator*(const Matrix4&, const Vector3&) noexcept;
	Y_API Vector4 operator*(const Matrix4&, const Vector4&) noexcept;
}

#endif
