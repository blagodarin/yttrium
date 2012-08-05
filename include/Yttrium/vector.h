/// \file
/// \brief Geometry/color vectors.

#ifndef __Y_VECTOR_H
#define __Y_VECTOR_H

#include <Yttrium/types.h>

namespace Yttrium
{

/// 2-component vector.

template <typename T>
class Vector2
{
public:

	union
	{
		struct
		{
			T x;
			T y;
		};
		struct
		{
			T r;
			T g;
		};
		struct
		{
			T s;
			T t;
		};
		struct
		{
			T width;
			T height;
		};
		struct
		{
			T left;
			T top;
		};
	};

public:

	Vector2(T x = 0, T y = 0)
		: x(x)
		, y(y)
	{
	}

	template <typename U>
	Vector2(U x, U y = 0)
		: x(x)
		, y(y)
	{
	}

	template <typename U>
	Vector2(const Vector2<U> &v)
		: x(v.x)
		, y(v.y)
	{
	}

public:

	Vector2 operator +(T t) const
	{
		return Vector2(x + t, y + t);
	}

	Vector2 &operator +=(T t)
	{
		x += t;
		y += t;
		return *this;
	}

	Vector2 operator +(const Vector2 &v) const
	{
		return Vector2(x + v.x, y + v.y);
	}

	Vector2 &operator +=(const Vector2 &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2 operator -(T t) const
	{
		return Vector2(x - t, y - t);
	}

	Vector2 &operator -=(T t)
	{
		x -= t;
		y -= t;
		return *this;
	}

	Vector2 operator -(const Vector2 &v) const
	{
		return Vector2(x - v.x, y - v.y);
	}

	Vector2 &operator -=(const Vector2 &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2 operator *(T t) const
	{
		return Vector2(x * t, y * t);
	}

	Vector2 &operator *=(T t)
	{
		x *= t;
		y *= t;
		return *this;
	}

	Vector2 operator *(const Vector2 &v) const
	{
		return Vector2(x * v.x, y * v.y);
	}

	Vector2 &operator *=(const Vector2 &v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vector2 operator /(T t) const
	{
		return Vector2(x / t, y / t);
	}

	Vector2 &operator /=(T t)
	{
		x /= t;
		y /= t;
		return *this;
	}

	Vector2 operator /(const Vector2 &v) const
	{
		return Vector2(x / v.x, y / v.y);
	}

	Vector2 &operator /=(const Vector2 &v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}
};

///

typedef Vector2<int8_t>  Vector2i8;

///

typedef Vector2<uint8_t> Vector2u8;

///

typedef Vector2<float>  Vector2f;

///

typedef Vector2<double> Vector2d;

///

typedef Vector2<Dim> Dim2;

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

	Vector3(T x = 0, T y = 0, T z = 0)
		: x(x)
		, y(y)
		, z(z)
	{
	}

	template <typename U>
	Vector3(U x, U y = 0, U z = 0)
		: x(x)
		, y(y)
		, z(z)
	{
	}

	template <typename U>
	Vector3(const Vector3<U> &v)
		: x(v.x)
		, y(v.y)
		, z(v.z)
	{
	}

public:

	Vector3 operator +(T t) const
	{
		return Vector3(x + t, y + t, z + t);
	}

	Vector3 &operator +=(T t)
	{
		x += t;
		y += t;
		z += t;
		return *this;
	}

	Vector3 operator +(const Vector3 &v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 &operator +=(const Vector3 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3 operator -(T t) const
	{
		return Vector3(x - t, y - t, z - t);
	}

	Vector3 &operator -=(T t)
	{
		x -= t;
		y -= t;
		z -= t;
		return *this;
	}

	Vector3 operator -(const Vector3 &v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 &operator -=(const Vector3 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3 operator *(T t) const
	{
		return Vector3(x * t, y * t, z * t);
	}

	Vector3 &operator *=(T t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	Vector3 operator *(const Vector3 &v) const
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	Vector3 &operator *=(const Vector3 &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
};

typedef Vector3<float> Vector3f; ///<

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

	Vector4(T x = 0, T y = 0, T z = 0, T w = 1)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{
	}

	template <typename U>
	Vector4(U x, U y = 0, U z = 0, U w = 1)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{
	}

	template <typename U>
	Vector4(const Vector4<U> &v)
		: x(v.x)
		, y(v.y)
		, z(v.z)
		, w(v.w)
	{
	}

public:

	Vector4 operator +(T t) const
	{
		return Vector4(x + t, y + t, z + t, w);
	}

	Vector4 &operator +=(T t)
	{
		x += t;
		y += t;
		z += t;
		return *this;
	}

	Vector4 operator +(const Vector4 &v) const
	{
		return Vector4(x + v.x, y + v.y, z + v.z, w);
	}

	Vector4 &operator +=(const Vector4 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector4 operator -(T t) const
	{
		return Vector4(x - t, y - t, z - t, w);
	}

	Vector4 &operator -=(T t)
	{
		x -= t;
		y -= t;
		z -= t;
		return *this;
	}

	Vector4 operator -(const Vector4 &v) const
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w);
	}

	Vector4 &operator -=(const Vector4 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector4 operator *(T t) const
	{
		return Vector4(x * t, y * t, z * t, w);
	}

	Vector4 &operator *=(T t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	Vector4 operator *(const Vector4 &v) const
	{
		return Vector4(x * v.x, y * v.y, z * v.z, w);
	}

	Vector4 &operator *=(const Vector4 &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
};

typedef Vector4<float> Vector4f; ///<

} // namespace Yttrium

#endif // __Y_VECTOR_H
