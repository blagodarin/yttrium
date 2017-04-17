#ifndef _include_yttrium_math_vector2_h_
#define _include_yttrium_math_vector2_h_

namespace Yttrium
{
	class Vector2
	{
	public:
		float x;
		float y;

		Vector2() noexcept = default;
		constexpr Vector2(float vx, float vy) noexcept : x{vx}, y{vy} {}

		constexpr auto& operator+=(float s) noexcept { x += s; y += s; return *this; }
		constexpr auto& operator+=(const Vector2& v) noexcept { x += v.x; y += v.y; return *this; }

		constexpr auto& operator-=(float s) noexcept { x -= s; y -= s; return *this; }
		constexpr auto& operator-=(const Vector2& v) noexcept { x -= v.x; y -= v.y; return *this; }

		constexpr auto& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
		constexpr auto& operator*=(const Vector2& v) noexcept { x *= v.x; y *= v.y; return *this; }

		constexpr auto& operator/=(float s) noexcept { x /= s; y /= s; return *this; }
		constexpr auto& operator/=(const Vector2& v) noexcept { x /= v.x; y /= v.y; return *this; }

		float* data() noexcept { return &x; }
		const float* data() const noexcept { return &x; }
	};

	constexpr Vector2 operator+(const Vector2& a, const Vector2& b) noexcept { return {a.x + b.x, a.y + b.y}; }
	constexpr Vector2 operator+(const Vector2& a, float b) noexcept { return {a.x + b, a.y + b}; }
	constexpr Vector2 operator+(float a, const Vector2& b) noexcept { return b + a; }

	constexpr Vector2 operator-(const Vector2& v) noexcept { return {-v.x, -v.y}; }

	constexpr Vector2 operator-(const Vector2& a, const Vector2& b) noexcept { return {a.x - b.x, a.y - b.y}; }
	constexpr Vector2 operator-(const Vector2& a, float b) noexcept { return {a.x - b, a.y - b}; }
	constexpr Vector2 operator-(float a, const Vector2& b) noexcept { return {a - b.x, a - b.y}; }

	constexpr Vector2 operator*(const Vector2& a, const Vector2& b) noexcept { return {a.x * b.x, a.y * b.y}; }
	constexpr Vector2 operator*(const Vector2& a, float b) noexcept { return {a.x * b, a.y * b}; }
	constexpr Vector2 operator*(float a, const Vector2& b) noexcept { return b * a; }

	constexpr Vector2 operator/(const Vector2& a, const Vector2& b) noexcept { return {a.x / b.x, a.y / b.y}; }
	constexpr Vector2 operator/(const Vector2& a, float b) noexcept { return {a.x / b, a.y / b}; }
	constexpr Vector2 operator/(float a, const Vector2& b) noexcept { return {a / b.x, a / b.y}; }
}

#endif
