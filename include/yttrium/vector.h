/// \file
/// \brief Geometry/color vectors.

#ifndef __Y_VECTOR_H
#define __Y_VECTOR_H

#include <yttrium/types.h>

namespace Yttrium
{

/// 2-component vector.

template <typename T>
class Vector2
{
public:

	T x;
	T y;

public:

	///

	Vector2() {}

	///

	Vector2(int) noexcept;

	///

	Vector2(T x, T y) noexcept;

	///

	template <typename U>
	Vector2(const Vector2<U> &vector) noexcept;

public:

	///

	T *data() noexcept;

	///

	const T *data() const noexcept;

public:

	///

	Vector2 operator +(T value) const noexcept;

	///

	Vector2 operator +(const Vector2 &vector) const noexcept;

	///

	Vector2 &operator +=(T value) noexcept;

	///

	Vector2 &operator +=(const Vector2 &vector) noexcept;

	///

	Vector2 operator -(T value) const noexcept;

	///

	Vector2 operator -(const Vector2 &vector) const noexcept;

	///

	Vector2 &operator -=(T value) noexcept;

	///

	Vector2 &operator -=(const Vector2 &vector) noexcept;

	///

	Vector2 operator *(T value) const noexcept;

	///

	Vector2 operator *(const Vector2 &vector) const noexcept;

	///

	Vector2 &operator *=(T value) noexcept;

	///

	Vector2 &operator *=(const Vector2 &vector) noexcept;

	///

	Vector2 operator /(T value) const noexcept;

	///

	Vector2 operator /(const Vector2 &vector) const noexcept;

	///

	Vector2 &operator /=(T value) noexcept;

	///

	Vector2 &operator /=(const Vector2 &vector) noexcept;
};

/// 3-component vector.

template <typename T>
class Vector3
{
public:

	union
	{
		struct
		{
			T x;
			T y;
			T z;
		};
		struct
		{
			T r;
			T g;
			T b;
		};
		struct
		{
			T s;
			T t;
			T p;
		};
	};

public:

	///

	Vector3() {}

	///

	Vector3(int) noexcept;

	///

	Vector3(T x, T y, T z) noexcept;

	///

	template <typename U>
	Vector3(const Vector3<U> &vector) noexcept;

public:

	///

	T *data() noexcept;

	///

	const T *data() const noexcept;

	///

	Vector2<T> xy() const noexcept;

public:

	///

	Vector3 operator +(T value) const noexcept;

	///

	Vector3 operator +(const Vector3 &vector) const noexcept;

	///

	Vector3 &operator +=(T value) noexcept;

	///

	Vector3 &operator +=(const Vector3 &vector) noexcept;

	///

	Vector3 operator -(T value) const noexcept;

	///

	Vector3 operator -(const Vector3 &vector) const noexcept;

	///

	Vector3 &operator -=(T value) noexcept;

	///

	Vector3 &operator -=(const Vector3 &vector) noexcept;

	///

	Vector3 operator *(T value) const noexcept;

	///

	Vector3 operator *(const Vector3 &vector) const noexcept;

	///

	Vector3 &operator *=(T value) noexcept;

	///

	Vector3 &operator *=(const Vector3 &vector) noexcept;
};

/// 4-component vector.

// NOTE: The w component is now unaffected by +'s and -'s. Is it right?

template <typename T>
class Vector4
{
public:

	union
	{
		struct
		{
			T x;
			T y;
			T z;
			T w;
		};
		struct
		{
			T r;
			T g;
			T b;
			T a;
		};
		struct
		{
			T s;
			T t;
			T p;
			T q;
		};
	};

public:

	///

	Vector4() {}

	///

	Vector4(int) noexcept;

	///

	Vector4(T x, T y, T z, T w = 1) noexcept;

	///

	template <typename U>
	Vector4(const Vector4<U> &vector) noexcept;

public:

	///

	T *data() noexcept;

	///

	const T *data() const noexcept;

public:

	///

	Vector4 operator +(T value) const noexcept;

	///

	Vector4 operator +(const Vector4 &vector) const noexcept;

	///

	Vector4 &operator +=(T value) noexcept;

	///

	Vector4 &operator +=(const Vector4 &vector) noexcept;

	///

	Vector4 operator -(T value) const noexcept;

	///

	Vector4 operator -(const Vector4 &vector) const noexcept;

	///

	Vector4 &operator -=(T value) noexcept;

	///

	Vector4 &operator -=(const Vector4 &vector) noexcept;

	///

	Vector4 operator *(T value) const noexcept;

	///

	Vector4 operator *(const Vector4 &vector) const noexcept;

	///

	Vector4 &operator *=(T value) noexcept;

	///

	Vector4 &operator *=(const Vector4 &vector) noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Vector2<T>::Vector2(int) noexcept
	: x(0)
	, y(0)
{
}

template <typename T>
Vector2<T>::Vector2(T x, T y) noexcept
	: x(x)
	, y(y)
{
}

template <typename T>
template <typename U>
Vector2<T>::Vector2(const Vector2<U> &vector) noexcept
	: x(vector.x)
	, y(vector.y)
{
}

template <typename T>
T *Vector2<T>::data() noexcept
{
	return &x;
}

template <typename T>
const T *Vector2<T>::data() const noexcept
{
	return &x;
}

template <typename T>
Vector2<T> Vector2<T>::operator +(T value) const noexcept
{
	return Vector2(x + value, y + value);
}

template <typename T>
Vector2<T> Vector2<T>::operator +(const Vector2 &vector) const noexcept
{
	return Vector2(x + vector.x, y + vector.y);
}

template <typename T>
Vector2<T> &Vector2<T>::operator +=(T value) noexcept
{
	x += value;
	y += value;
	return *this;
}

template <typename T>
Vector2<T> &Vector2<T>::operator +=(const Vector2 &vector) noexcept
{
	x += vector.x;
	y += vector.y;
	return *this;
}

template <typename T>
Vector2<T> Vector2<T>::operator -(T value) const noexcept
{
	return Vector2(x - value, y - value);
}

template <typename T>
Vector2<T> Vector2<T>::operator -(const Vector2 &vector) const noexcept
{
	return Vector2(x - vector.x, y - vector.y);
}

template <typename T>
Vector2<T> &Vector2<T>::operator -=(T value) noexcept
{
	x -= value;
	y -= value;
	return *this;
}

template <typename T>
Vector2<T> &Vector2<T>::operator -=(const Vector2 &vector) noexcept
{
	x -= vector.x;
	y -= vector.y;
	return *this;
}

template <typename T>
Vector2<T> Vector2<T>::operator *(T value) const noexcept
{
	return Vector2(x * value, y * value);
}

template <typename T>
Vector2<T> Vector2<T>::operator *(const Vector2 &vector) const noexcept
{
	return Vector2(x * vector.x, y * vector.y);
}

template <typename T>
Vector2<T> &Vector2<T>::operator *=(T value) noexcept
{
	x *= value;
	y *= value;
	return *this;
}

template <typename T>
Vector2<T> &Vector2<T>::operator *=(const Vector2 &vector) noexcept
{
	x *= vector.x;
	y *= vector.y;
	return *this;
}

template <typename T>
Vector2<T> Vector2<T>::operator /(T value) const noexcept
{
	return Vector2(x / value, y / value);
}

template <typename T>
Vector2<T> Vector2<T>::operator /(const Vector2 &vector) const noexcept
{
	return Vector2(x / vector.x, y / vector.y);
}

template <typename T>
Vector2<T> &Vector2<T>::operator /=(T value) noexcept
{
	x /= value;
	y /= value;
	return *this;
}

template <typename T>
Vector2<T> &Vector2<T>::operator /=(const Vector2 &vector) noexcept
{
	x /= vector.x;
	y /= vector.y;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Vector3<T>::Vector3(int) noexcept
	: x(0)
	, y(0)
	, z(0)
{
}

template <typename T>
Vector3<T>::Vector3(T x, T y, T z) noexcept
	: x(x)
	, y(y)
	, z(z)
{
}

template <typename T>
template <typename U>
Vector3<T>::Vector3(const Vector3<U> &vector) noexcept
	: x(vector.x)
	, y(vector.y)
	, z(vector.z)
{
}

template <typename T>
T *Vector3<T>::data() noexcept
{
	return &x;
}

template <typename T>
const T *Vector3<T>::data() const noexcept
{
	return &x;
}

template <typename T>
Vector2<T> Vector3<T>::xy() const noexcept
{
	return Vector2<T>(x, y);
}

template <typename T>
Vector3<T> Vector3<T>::operator +(T value) const noexcept
{
	return Vector3(x + value, y + value, z + value);
}

template <typename T>
Vector3<T> Vector3<T>::operator +(const Vector3 &vector) const noexcept
{
	return Vector3(x + vector.x, y + vector.y, z + vector.z);
}

template <typename T>
Vector3<T> &Vector3<T>::operator +=(T value) noexcept
{
	x += value;
	y += value;
	z += value;
	return *this;
}

template <typename T>
Vector3<T> &Vector3<T>::operator +=(const Vector3 &vector) noexcept
{
	x += vector.x;
	y += vector.y;
	z += vector.z;
	return *this;
}

template <typename T>
Vector3<T> Vector3<T>::operator -(T value) const noexcept
{
	return Vector3(x - value, y - value, z - value);
}

template <typename T>
Vector3<T> Vector3<T>::operator -(const Vector3 &vector) const noexcept
{
	return Vector3(x - vector.x, y - vector.y, z - vector.z);
}

template <typename T>
Vector3<T> &Vector3<T>::operator -=(T value) noexcept
{
	x -= value;
	y -= value;
	z -= value;
	return *this;
}

template <typename T>
Vector3<T> &Vector3<T>::operator -=(const Vector3 &vector) noexcept
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;
	return *this;
}

template <typename T>
Vector3<T> Vector3<T>::operator *(T value) const noexcept
{
	return Vector3(x * value, y * value, z * value);
}

template <typename T>
Vector3<T> Vector3<T>::operator *(const Vector3 &vector) const noexcept
{
	return Vector3(x * vector.x, y * vector.y, z * vector.z);
}

template <typename T>
Vector3<T> &Vector3<T>::operator *=(T value) noexcept
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

template <typename T>
Vector3<T> &Vector3<T>::operator *=(const Vector3 &vector) noexcept
{
	x *= vector.x;
	y *= vector.y;
	z *= vector.z;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Vector4<T>::Vector4(int) noexcept
	: x(0)
	, y(0)
	, z(0)
	, w(1)
{
}

template <typename T>
Vector4<T>::Vector4(T x, T y, T z, T w) noexcept
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{
}

template <typename T>
template <typename U>
Vector4<T>::Vector4(const Vector4<U> &vector) noexcept
	: x(vector.x)
	, y(vector.y)
	, z(vector.z)
	, w(vector.w)
{
}

template <typename T>
T *Vector4<T>::data() noexcept
{
	return &x;
}

template <typename T>
const T *Vector4<T>::data() const noexcept
{
	return &x;
}

template <typename T>
Vector4<T> Vector4<T>::operator +(T value) const noexcept
{
	return Vector4(x + value, y + value, z + value, w);
}

template <typename T>
Vector4<T> Vector4<T>::operator +(const Vector4 &vector) const noexcept
{
	return Vector4(x + vector.x, y + vector.y, z + vector.z, w);
}

template <typename T>
Vector4<T> &Vector4<T>::operator +=(T value) noexcept
{
	x += value;
	y += value;
	z += value;
	return *this;
}

template <typename T>
Vector4<T> &Vector4<T>::operator +=(const Vector4 &vector) noexcept
{
	x += vector.x;
	y += vector.y;
	z += vector.z;
	return *this;
}

template <typename T>
Vector4<T> Vector4<T>::operator -(T value) const noexcept
{
	return Vector4(x - value, y - value, z - value, w);
}

template <typename T>
Vector4<T> Vector4<T>::operator -(const Vector4 &vector) const noexcept
{
	return Vector4(x - vector.x, y - vector.y, z - vector.z, w);
}

template <typename T>
Vector4<T> &Vector4<T>::operator -=(T value) noexcept
{
	x -= value;
	y -= value;
	z -= value;
	return *this;
}

template <typename T>
Vector4<T> &Vector4<T>::operator -=(const Vector4 &vector) noexcept
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;
	return *this;
}

template <typename T>
Vector4<T> Vector4<T>::operator *(T value) const noexcept
{
	return Vector4(x * value, y * value, z * value, w);
}

template <typename T>
Vector4<T> Vector4<T>::operator *(const Vector4 &vector) const noexcept
{
	return Vector4(x * vector.x, y * vector.y, z * vector.z, w);
}

template <typename T>
Vector4<T> &Vector4<T>::operator *=(T value) noexcept
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

template <typename T>
Vector4<T> &Vector4<T>::operator *=(const Vector4 &vector) noexcept
{
	x *= vector.x;
	y *= vector.y;
	z *= vector.z;
	return *this;
}

} // namespace Yttrium

#endif // __Y_VECTOR_H
