#ifndef _include_yttrium_math_vector3_h_
#define _include_yttrium_math_vector3_h_

#include <cmath>

namespace Yttrium
{
	class Vector3
	{
	public:
		float x;
		float y;
		float z;

		Vector3() noexcept = default;
		constexpr Vector3(float vx, float vy, float vz) noexcept : x{vx}, y{vy}, z{vz} {}

		constexpr auto& operator+=(float s) noexcept { x += s; y += s; z += s; return *this; }
		constexpr auto& operator+=(const Vector3& v) noexcept { x += v.x; y += v.y; z += v.z; return *this; }

		constexpr auto& operator-=(float s) noexcept { x -= s; y -= s; z -= s; return *this; }
		constexpr auto& operator-=(const Vector3& v) noexcept { x -= v.x; y -= v.y; z -= v.z; return *this; }

		constexpr auto& operator*=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }
		constexpr auto& operator*=(const Vector3& v) noexcept { x *= v.x; y *= v.y; z *= v.z; return *this; }

		constexpr auto& operator/=(float s) noexcept { x /= s; y /= s; z /= s; return *this; }
		constexpr auto& operator/=(const Vector3& v) noexcept { x /= v.x; y /= v.y; z /= v.z; return *this; }

		float length() const noexcept
		{
			return std::sqrt(x * x + y * y + z * z);
		}

		Vector3 normalized() const noexcept
		{
			auto v = *this;
			v /= length();
			return v;
		}
	};

	constexpr Vector3 operator+(const Vector3& a, const Vector3& b) noexcept { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
	constexpr Vector3 operator+(const Vector3& a, float b) noexcept { return {a.x + b, a.y + b, a.z + b}; }
	constexpr Vector3 operator+(float a, const Vector3& b) noexcept { return b + a; }

	constexpr Vector3 operator-(const Vector3& v) noexcept { return {-v.x, -v.y, -v.z}; }

	constexpr Vector3 operator-(const Vector3& a, const Vector3& b) noexcept { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
	constexpr Vector3 operator-(const Vector3& a, float b) noexcept { return {a.x - b, a.y - b, a.z - b}; }
	constexpr Vector3 operator-(float a, const Vector3& b) noexcept { return {a - b.x, a - b.y, a - b.z}; }

	constexpr Vector3 operator*(const Vector3& a, const Vector3& b) noexcept { return {a.x * b.x, a.y * b.y, a.z * b.z}; }
	constexpr Vector3 operator*(const Vector3& a, float b) noexcept { return {a.x * b, a.y * b, a.z * b}; }
	constexpr Vector3 operator*(float a, const Vector3& b) noexcept { return b * a; }

	constexpr Vector3 operator/(const Vector3& a, const Vector3& b) noexcept { return {a.x / b.x, a.y / b.y, a.z / b.z}; }
	constexpr Vector3 operator/(const Vector3& a, float b) noexcept { return {a.x / b, a.y / b, a.z / b}; }
	constexpr Vector3 operator/(float a, const Vector3& b) noexcept { return {a / b.x, a / b.y, a / b.z}; }

	constexpr float dot_product(const Vector3& a, const Vector3& b) noexcept
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
}

#endif
