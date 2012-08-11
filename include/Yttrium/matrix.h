/// \file
/// \brief

#ifndef __Y_MATRIX_H
#define __Y_MATRIX_H

#include <Yttrium/vector.h>

namespace Yttrium
{

///

class Matrix4f
{
public:

	///

	Matrix4f() noexcept;

	///

	Matrix4f(float m00, float m01, float m02, float m03, float m10, float m11,
		float m12, float m13, float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) noexcept;

	///

	explicit Matrix4f(const float *data) noexcept;

public:

	///

	const float *data() const noexcept
	{
		return _data[0];
	}

public:

	///

	static Matrix4f projection_2d(float left, float top, float width, float height, float near = -1.f, float far = 1.f) noexcept;

private:

	Matrix4f(int) noexcept;

private:

	float _data[4][4];
};

////////////////////////////////////////////////////////////////////////////////

inline Matrix4f::Matrix4f()
{
	_data[0][0] = 1.f;
	_data[0][1] = 0.f;
	_data[0][2] = 0.f;
	_data[0][3] = 0.f;

	_data[1][0] = 0.f;
	_data[1][1] = 1.f;
	_data[1][2] = 0.f;
	_data[1][3] = 0.f;

	_data[2][0] = 0.f;
	_data[2][1] = 0.f;
	_data[2][2] = 1.f;
	_data[2][3] = 0.f;

	_data[3][0] = 0.f;
	_data[3][1] = 0.f;
	_data[3][2] = 0.f;
	_data[3][3] = 1.f;
}

inline Matrix4f::Matrix4f(float m00, float m01, float m02, float m03, float m10,
	float m11, float m12, float m13, float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	_data[0][0] = m00;
	_data[0][1] = m10;
	_data[0][2] = m20;
	_data[0][3] = m30;

	_data[1][0] = m01;
	_data[1][1] = m11;
	_data[1][2] = m21;
	_data[1][3] = m31;

	_data[2][0] = m02;
	_data[2][1] = m12;
	_data[2][2] = m22;
	_data[2][3] = m32;

	_data[3][0] = m03;
	_data[3][1] = m13;
	_data[3][2] = m23;
	_data[3][3] = m33;
}

inline Matrix4f::Matrix4f(const float *data)
{
	_data[0][0] = data[0];
	_data[0][1] = data[1];
	_data[0][2] = data[2];
	_data[0][3] = data[3];

	_data[1][0] = data[4];
	_data[1][1] = data[5];
	_data[1][2] = data[6];
	_data[1][3] = data[7];

	_data[2][0] = data[8];
	_data[2][1] = data[9];
	_data[2][2] = data[10];
	_data[2][3] = data[11];

	_data[3][0] = data[12];
	_data[3][1] = data[13];
	_data[3][2] = data[14];
	_data[3][3] = data[15];
}

inline Matrix4f::Matrix4f(int)
{
}

} // namespace Yttrium

#endif // __Y_MATRIX_H
