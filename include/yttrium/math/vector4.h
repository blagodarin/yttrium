#ifndef _include_yttrium_math_vector4_h_
#define _include_yttrium_math_vector4_h_

#include <yttrium/math/vector3.h>

namespace Yttrium
{
	class Vector4
	{
	public:
		float x, y, z, w;

		Vector4() noexcept = default;
		constexpr Vector4(float vx, float vy, float vz, float vw) noexcept : x{vx}, y{vy}, z{vz}, w{vw} {}
		constexpr Vector4(const Vector3& v, float vw) noexcept : x{v.x}, y{v.y}, z{v.z}, w{vw} {}

		constexpr auto& operator+=(float s) noexcept { x += s; y += s; z += s; w += s; return *this; }
		constexpr auto& operator+=(const Vector4& v) noexcept { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }

		constexpr auto& operator-=(float s) noexcept { x -= s; y -= s; z -= s; w -= s; return *this; }
		constexpr auto& operator-=(const Vector4& v) noexcept { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }

		constexpr auto& operator*=(float s) noexcept { x *= s; y *= s; z *= s; w *= s; return *this; }
		constexpr auto& operator*=(const Vector4& v) noexcept { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }

		constexpr auto& operator/=(float s) { x /= s; y /= s; z /= s; w /= s; return *this; }
		constexpr auto& operator/=(const Vector4& v) noexcept { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
	};

	constexpr Vector4 operator+(const Vector4& a, const Vector4& b) noexcept { return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }
	constexpr Vector4 operator+(const Vector4& a, float b) noexcept { return {a.x + b, a.y + b, a.z + b, a.w + b}; }
	constexpr Vector4 operator+(float a, const Vector4& b) noexcept { return b + a; }

	constexpr Vector4 operator-(const Vector4& v) noexcept { return {-v.x, -v.y, -v.z, -v.w}; }

	constexpr Vector4 operator-(const Vector4& a, const Vector4& b) noexcept { return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }
	constexpr Vector4 operator-(const Vector4& a, float b) noexcept { return {a.x - b, a.y - b, a.z - b, a.w - b}; }
	constexpr Vector4 operator-(float a, const Vector4& b) noexcept { return {a - b.x, a - b.y, a - b.z, a - b.w}; }

	constexpr Vector4 operator*(const Vector4& a, const Vector4& b) noexcept { return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}; }
	constexpr Vector4 operator*(const Vector4& a, float b) noexcept { return {a.x * b, a.y * b, a.z * b, a.w * b}; }
	constexpr Vector4 operator*(float a, const Vector4& b) noexcept { return b * a; }

	constexpr Vector4 operator/(const Vector4& a, const Vector4& b) noexcept { return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}; }
	constexpr Vector4 operator/(const Vector4& a, float b) noexcept { return {a.x / b, a.y / b, a.z / b, a.w / b}; }
	constexpr Vector4 operator/(float a, const Vector4& b) noexcept { return {a / b.x, a / b.y, a / b.z, a / b.w}; }
}

#endif
