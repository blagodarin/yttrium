/// \file
/// \brief Geometry/color vectors.

#ifndef __Y_VECTOR_H
#define __Y_VECTOR_H

#include <yttrium/global.h>
#include <yttrium/point.h>
#include <yttrium/size.h>

namespace Yttrium
{
	/// 2-component 2D vector.
	class Vector2
	{
	public:

		float x; ///<
		float y; ///<

		///
		Vector2() = default;

		///
		Vector2(float x, float y): x(x), y(y) {}

		///
		explicit Vector2(const Point& point)
			: x(point.x)
			, y(point.y)
		{
		}

		///
		explicit Vector2(const Size& size)
			: x(size.width)
			, y(size.height)
		{
		}

		///
		float* data() { return &x; }

		///
		const float* data() const { return &x; }

		///
		Vector2& operator+=(float scalar)
		{
			x += scalar;
			y += scalar;
			return *this;
		}

		///
		Vector2& operator+=(const Vector2& vector)
		{
			x += vector.x;
			y += vector.y;
			return *this;
		}

		///
		Vector2& operator-=(float scalar)
		{
			x -= scalar;
			y -= scalar;
			return *this;
		}

		///
		Vector2& operator-=(const Vector2& vector)
		{
			x -= vector.x;
			y -= vector.y;
			return *this;
		}

		///
		Vector2& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		///
		Vector2& operator*=(const Vector2& vector)
		{
			x *= vector.x;
			y *= vector.y;
			return *this;
		}

		///
		Vector2& operator/=(float scalar)
		{
			x /= scalar;
			y /= scalar;
			return *this;
		}

		///
		Vector2& operator/=(const Vector2& vector)
		{
			x /= vector.x;
			y /= vector.y;
			return *this;
		}
	};

	/// 4-component 3D vector.
	class Y_API Vector4
	{
	public:

		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			struct
			{
				float r;
				float g;
				float b;
				float a;
			};
			struct
			{
				float s;
				float t;
				float p;
				float q;
			};
		};

		///
		Vector4() = default;

		///
		Vector4(float x, float y, float z, float w = 1): x(x), y(y), z(z), w(w) {}

		///
		float* data() { return &x; }
		const float* data() const { return &x; }

		///
		float length() const;

		///
		Vector4 normalized() const;

		///
		Vector4& operator+=(float scalar)
		{
			x += scalar;
			y += scalar;
			z += scalar;
			return *this;
		}

		///
		Vector4& operator+=(const Vector4& vector)
		{
			x += vector.x;
			y += vector.y;
			z += vector.z;
			return *this;
		}

		///
		Vector4& operator-=(float scalar)
		{
			x -= scalar;
			y -= scalar;
			z -= scalar;
			return *this;
		}

		///
		Vector4& operator-=(const Vector4& vector)
		{
			x -= vector.x;
			y -= vector.y;
			z -= vector.z;
			return *this;
		}

		///
		Vector4& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		///
		Vector4& operator*=(const Vector4& vector)
		{
			x *= vector.x;
			y *= vector.y;
			z *= vector.z;
			return *this;
		}

		///
		Vector4& operator/=(float scalar)
		{
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}

		///
		Vector4& operator/=(const Vector4& vector)
		{
			x /= vector.x;
			y /= vector.y;
			z /= vector.z;
			return *this;
		}
	};

	inline Vector2 operator+(Vector2 left, const Vector2& right) { return left += right; }
	inline Vector2 operator+(Vector2 left, float right) { return left += right; }
	inline Vector2 operator+(float left, Vector2 right) { return right += left; }

	inline Vector2 operator-(Vector2 left, const Vector2& right) { return left -= right; }
	inline Vector2 operator-(Vector2 left, float right) { return left -= right; }
	inline Vector2 operator-(float left, const Vector2& right) { return Vector2(left - right.x, left - right.y); }

	inline Vector2 operator*(Vector2 left, const Vector2& right) { return left *= right; }
	inline Vector2 operator*(Vector2 left, float right) { return left *= right; }
	inline Vector2 operator*(float left, Vector2 right) { return right *= left; }

	inline Vector2 operator/(Vector2 left, const Vector2& right) { return left /= right; }
	inline Vector2 operator/(Vector2 left, float right) { return left /= right; }
	inline Vector2 operator/(float left, const Vector2& right) { return Vector2(left / right.x, left / right.y); }

	inline bool operator==(const Vector2& left, const Vector2& right)
	{
		return left.x == right.x && left.y == right.y;
	}

	inline Vector4 operator-(const Vector4& vector)
	{
		return Vector4(-vector.x, -vector.y, -vector.z);
	}

	inline Vector4 operator+(const Vector4& left, const Vector4& right)
	{
		return Vector4(left.x + right.x, left.y + right.y, left.z + right.z);
	}

	inline Vector4 operator+(const Vector4& left, float right)
	{
		return Vector4(left.x + right, left.y + right, left.z + right);
	}

	inline Vector4 operator+(float left, const Vector4& right)
	{
		return Vector4(left + right.x, left + right.y, left + right.z);
	}

	inline Vector4 operator-(const Vector4& left, const Vector4& right)
	{
		return Vector4(left.x - right.x, left.y - right.y, left.z - right.z);
	}

	inline Vector4 operator-(const Vector4& left, float right)
	{
		return Vector4(left.x - right, left.y - right, left.z - right);
	}

	inline Vector4 operator-(float left, const Vector4& right)
	{
		return Vector4(left - right.x, left - right.y, left - right.z);
	}

	inline Vector4 operator*(const Vector4& left, const Vector4& right)
	{
		return Vector4(left.x * right.x, left.y * right.y, left.z * right.z);
	}

	inline Vector4 operator*(const Vector4& left, float right)
	{
		return Vector4(left.x * right, left.y * right, left.z * right);
	}

	inline Vector4 operator*(float left, const Vector4& right)
	{
		return Vector4(left * right.x, left * right.y, left * right.z);
	}

	inline Vector4 operator/(const Vector4& left, const Vector4& right)
	{
		return Vector4(left.x / right.x, left.y / right.y, left.z / right.z);
	}

	inline Vector4 operator/(const Vector4& left, float right)
	{
		return Vector4(left.x / right, left.y / right, left.z / right);
	}

	inline Vector4 operator/(float left, const Vector4& right)
	{
		return Vector4(left / right.x, left / right.y, left / right.z);
	}

	inline bool operator==(const Vector4& left, const Vector4& right)
	{
		return left.x == right.x && left.y == right.y && left.z == right.z;
	}
}

#endif // __Y_VECTOR_H
