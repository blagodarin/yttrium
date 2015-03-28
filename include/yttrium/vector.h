/// \file
/// \brief Geometry/color vectors.

#ifndef __Y_VECTOR_H
#define __Y_VECTOR_H

#include <yttrium/global.h>

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

	Vector2(int);

	///

	Vector2(T x, T y);

	///

	template <typename U>
	Vector2(const Vector2<U> &vector);

public:

	///

	T *data();

	///

	const T *data() const;

	///

	template <typename U>
	Vector2<U> to() const;

public:

	///

	Vector2 operator +(T value) const;

	///

	Vector2 operator +(const Vector2 &vector) const;

	///

	Vector2 &operator +=(T value);

	///

	Vector2 &operator +=(const Vector2 &vector);

	///

	Vector2 operator -(T value) const;

	///

	Vector2 operator -(const Vector2 &vector) const;

	///

	Vector2 &operator -=(T value);

	///

	Vector2 &operator -=(const Vector2 &vector);

	///

	Vector2 operator *(T value) const;

	///

	Vector2 operator *(const Vector2 &vector) const;

	///

	Vector2 &operator *=(T value);

	///

	Vector2 &operator *=(const Vector2 &vector);

	///

	Vector2 operator /(T value) const;

	///

	Vector2 operator /(const Vector2 &vector) const;

	///

	Vector2 &operator /=(T value);

	///

	Vector2 &operator /=(const Vector2 &vector);
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

	Vector4(int);

	///

	Vector4(T x, T y, T z, T w = 1);

	///

	template <typename U>
	Vector4(const Vector4<U> &vector);

public:

	///

	T *data();

	///

	const T *data() const;

public:

	///

	Vector4 operator +(T value) const;

	///

	Vector4 operator +(const Vector4 &vector) const;

	///

	Vector4 &operator +=(T value);

	///

	Vector4 &operator +=(const Vector4 &vector);

	///

	Vector4 operator -(T value) const;

	///

	Vector4 operator -(const Vector4 &vector) const;

	///

	Vector4 &operator -=(T value);

	///

	Vector4 &operator -=(const Vector4 &vector);

	///

	Vector4 operator *(T value) const;

	///

	Vector4 operator *(const Vector4 &vector) const;

	///

	Vector4 &operator *=(T value);

	///

	Vector4 &operator *=(const Vector4 &vector);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Vector2<T>::Vector2(int)
	: x(0)
	, y(0)
{
}

template <typename T>
Vector2<T>::Vector2(T x, T y)
	: x(x)
	, y(y)
{
}

template <typename T>
template <typename U>
Vector2<T>::Vector2(const Vector2<U> &vector)
	: x(vector.x)
	, y(vector.y)
{
}

template <typename T>
T *Vector2<T>::data()
{
	return &x;
}

template <typename T>
const T *Vector2<T>::data() const
{
	return &x;
}

template <typename T>
template <typename U>
Vector2<U> Vector2<T>::to() const
{
	return Vector2<U>(x, y);
}

template <typename T>
Vector2<T> Vector2<T>::operator +(T value) const
{
	return Vector2(x + value, y + value);
}

template <typename T>
Vector2<T> Vector2<T>::operator +(const Vector2 &vector) const
{
	return Vector2(x + vector.x, y + vector.y);
}

template <typename T>
Vector2<T> &Vector2<T>::operator +=(T value)
{
	x += value;
	y += value;
	return *this;
}

template <typename T>
Vector2<T> &Vector2<T>::operator +=(const Vector2 &vector)
{
	x += vector.x;
	y += vector.y;
	return *this;
}

template <typename T>
Vector2<T> Vector2<T>::operator -(T value) const
{
	return Vector2(x - value, y - value);
}

template <typename T>
Vector2<T> Vector2<T>::operator -(const Vector2 &vector) const
{
	return Vector2(x - vector.x, y - vector.y);
}

template <typename T>
Vector2<T> &Vector2<T>::operator -=(T value)
{
	x -= value;
	y -= value;
	return *this;
}

template <typename T>
Vector2<T> &Vector2<T>::operator -=(const Vector2 &vector)
{
	x -= vector.x;
	y -= vector.y;
	return *this;
}

template <typename T>
Vector2<T> Vector2<T>::operator *(T value) const
{
	return Vector2(x * value, y * value);
}

template <typename T>
Vector2<T> Vector2<T>::operator *(const Vector2 &vector) const
{
	return Vector2(x * vector.x, y * vector.y);
}

template <typename T>
Vector2<T> &Vector2<T>::operator *=(T value)
{
	x *= value;
	y *= value;
	return *this;
}

template <typename T>
Vector2<T> &Vector2<T>::operator *=(const Vector2 &vector)
{
	x *= vector.x;
	y *= vector.y;
	return *this;
}

template <typename T>
Vector2<T> Vector2<T>::operator /(T value) const
{
	return Vector2(x / value, y / value);
}

template <typename T>
Vector2<T> Vector2<T>::operator /(const Vector2 &vector) const
{
	return Vector2(x / vector.x, y / vector.y);
}

template <typename T>
Vector2<T> &Vector2<T>::operator /=(T value)
{
	x /= value;
	y /= value;
	return *this;
}

template <typename T>
Vector2<T> &Vector2<T>::operator /=(const Vector2 &vector)
{
	x /= vector.x;
	y /= vector.y;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Vector4<T>::Vector4(int)
	: x(0)
	, y(0)
	, z(0)
	, w(1)
{
}

template <typename T>
Vector4<T>::Vector4(T x, T y, T z, T w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{
}

template <typename T>
template <typename U>
Vector4<T>::Vector4(const Vector4<U> &vector)
	: x(vector.x)
	, y(vector.y)
	, z(vector.z)
	, w(vector.w)
{
}

template <typename T>
T *Vector4<T>::data()
{
	return &x;
}

template <typename T>
const T *Vector4<T>::data() const
{
	return &x;
}

template <typename T>
Vector4<T> Vector4<T>::operator +(T value) const
{
	return Vector4(x + value, y + value, z + value, w);
}

template <typename T>
Vector4<T> Vector4<T>::operator +(const Vector4 &vector) const
{
	return Vector4(x + vector.x, y + vector.y, z + vector.z, w);
}

template <typename T>
Vector4<T> &Vector4<T>::operator +=(T value)
{
	x += value;
	y += value;
	z += value;
	return *this;
}

template <typename T>
Vector4<T> &Vector4<T>::operator +=(const Vector4 &vector)
{
	x += vector.x;
	y += vector.y;
	z += vector.z;
	return *this;
}

template <typename T>
Vector4<T> Vector4<T>::operator -(T value) const
{
	return Vector4(x - value, y - value, z - value, w);
}

template <typename T>
Vector4<T> Vector4<T>::operator -(const Vector4 &vector) const
{
	return Vector4(x - vector.x, y - vector.y, z - vector.z, w);
}

template <typename T>
Vector4<T> &Vector4<T>::operator -=(T value)
{
	x -= value;
	y -= value;
	z -= value;
	return *this;
}

template <typename T>
Vector4<T> &Vector4<T>::operator -=(const Vector4 &vector)
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;
	return *this;
}

template <typename T>
Vector4<T> Vector4<T>::operator *(T value) const
{
	return Vector4(x * value, y * value, z * value, w);
}

template <typename T>
Vector4<T> Vector4<T>::operator *(const Vector4 &vector) const
{
	return Vector4(x * vector.x, y * vector.y, z * vector.z, w);
}

template <typename T>
Vector4<T> &Vector4<T>::operator *=(T value)
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

template <typename T>
Vector4<T> &Vector4<T>::operator *=(const Vector4 &vector)
{
	x *= vector.x;
	y *= vector.y;
	z *= vector.z;
	return *this;
}

} // namespace Yttrium

#endif // __Y_VECTOR_H
