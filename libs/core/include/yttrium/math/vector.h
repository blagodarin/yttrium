//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <yttrium/math/point.h>

#include <cmath>

namespace Yt
{
	class Vector2
	{
	public:
		float x, y;

		Vector2() noexcept = default;
		constexpr Vector2(float vx, float vy) noexcept
			: x{ vx }, y{ vy } {}
		constexpr explicit Vector2(const Point& p) noexcept
			: x{ static_cast<float>(p._x) }, y{ static_cast<float>(p._y) } {}

		// clang-format off
		constexpr auto& operator +=(const Vector2& v) noexcept { x += v.x; y += v.y; return *this; }
		constexpr auto& operator +=(float s) noexcept { x += s; y += s; return *this; }
		constexpr auto& operator -=(const Vector2& v) noexcept { x -= v.x; y -= v.y; return *this; }
		constexpr auto& operator -=(float s) noexcept { x -= s; y -= s; return *this; }
		constexpr auto& operator *=(const Vector2& v) noexcept { x *= v.x; y *= v.y; return *this; }
		constexpr auto& operator *=(float s) noexcept { x *= s; y *= s; return *this; }
		constexpr auto& operator /=(const Vector2& v) noexcept { x /= v.x; y /= v.y; return *this; }
		constexpr auto& operator /=(float s) noexcept { x /= s; y /= s; return *this; }
		// clang-format on
	};

	class Vector3
	{
	public:
		float x, y, z;

		Vector3() noexcept = default;
		constexpr Vector3(float vx, float vy, float vz) noexcept
			: x{ vx }, y{ vy }, z{ vz } {}
		constexpr Vector3(const Vector2& v, float vz) noexcept
			: x{ v.x }, y{ v.y }, z{ vz } {}

		// clang-format off
		constexpr auto& operator +=(const Vector3& v) noexcept { x += v.x; y += v.y; z += v.z; return *this; }
		constexpr auto& operator +=(float s) noexcept { x += s; y += s; z += s; return *this; }
		constexpr auto& operator -=(const Vector3& v) noexcept { x -= v.x; y -= v.y; z -= v.z; return *this; }
		constexpr auto& operator -=(float s) noexcept { x -= s; y -= s; z -= s; return *this; }
		constexpr auto& operator *=(const Vector3& v) noexcept { x *= v.x; y *= v.y; z *= v.z; return *this; }
		constexpr auto& operator *=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }
		constexpr auto& operator /=(const Vector3& v) noexcept { x /= v.x; y /= v.y; z /= v.z; return *this; }
		constexpr auto& operator /=(float s) noexcept { x /= s; y /= s; z /= s; return *this; }
		// clang-format on
	};

	class Vector4
	{
	public:
		float x, y, z, w;

		Vector4() noexcept = default;
		constexpr Vector4(float vx, float vy, float vz, float vw) noexcept
			: x{ vx }, y{ vy }, z{ vz }, w{ vw } {}
		constexpr Vector4(const Vector3& v, float vw) noexcept
			: x{ v.x }, y{ v.y }, z{ v.z }, w{ vw } {}

		// clang-format off
		constexpr auto& operator +=(const Vector4& v) noexcept { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		constexpr auto& operator +=(float s) noexcept { x += s; y += s; z += s; w += s; return *this; }
		constexpr auto& operator -=(const Vector4& v) noexcept { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		constexpr auto& operator -=(float s) noexcept { x -= s; y -= s; z -= s; w -= s; return *this; }
		constexpr auto& operator *=(const Vector4& v) noexcept { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
		constexpr auto& operator *=(float s) noexcept { x *= s; y *= s; z *= s; w *= s; return *this; }
		constexpr auto& operator /=(const Vector4& v) noexcept { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
		constexpr auto& operator /=(float s) noexcept { x /= s; y /= s; z /= s; w /= s; return *this; }
		// clang-format on
	};

	constexpr bool operator==(const Vector2& a, const Vector2& b) noexcept { return a.x == b.x && a.y == b.y; }
	constexpr bool operator==(const Vector3& a, const Vector3& b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z; }
	constexpr bool operator==(const Vector4& a, const Vector4& b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

	constexpr bool operator!=(const Vector2& a, const Vector2& b) noexcept { return !(a == b); }
	constexpr bool operator!=(const Vector3& a, const Vector3& b) noexcept { return !(a == b); }
	constexpr bool operator!=(const Vector4& a, const Vector4& b) noexcept { return !(a == b); }

	constexpr Vector2 operator+(const Vector2& a, const Vector2& b) noexcept { return { a.x + b.x, a.y + b.y }; }
	constexpr Vector3 operator+(const Vector3& a, const Vector3& b) noexcept { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
	constexpr Vector4 operator+(const Vector4& a, const Vector4& b) noexcept { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }

	constexpr Vector2 operator+(const Vector2& a, float b) noexcept { return { a.x + b, a.y + b }; }
	constexpr Vector3 operator+(const Vector3& a, float b) noexcept { return { a.x + b, a.y + b, a.z + b }; }
	constexpr Vector4 operator+(const Vector4& a, float b) noexcept { return { a.x + b, a.y + b, a.z + b, a.w + b }; }

	constexpr Vector2 operator+(float a, const Vector2& b) noexcept { return b + a; }
	constexpr Vector3 operator+(float a, const Vector3& b) noexcept { return b + a; }
	constexpr Vector4 operator+(float a, const Vector4& b) noexcept { return b + a; }

	constexpr Vector2 operator-(const Vector2& v) noexcept { return { -v.x, -v.y }; }
	constexpr Vector3 operator-(const Vector3& v) noexcept { return { -v.x, -v.y, -v.z }; }
	constexpr Vector4 operator-(const Vector4& v) noexcept { return { -v.x, -v.y, -v.z, -v.w }; }

	constexpr Vector2 operator-(const Vector2& a, const Vector2& b) noexcept { return { a.x - b.x, a.y - b.y }; }
	constexpr Vector3 operator-(const Vector3& a, const Vector3& b) noexcept { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
	constexpr Vector4 operator-(const Vector4& a, const Vector4& b) noexcept { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }

	constexpr Vector2 operator-(const Vector2& a, float b) noexcept { return { a.x - b, a.y - b }; }
	constexpr Vector3 operator-(const Vector3& a, float b) noexcept { return { a.x - b, a.y - b, a.z - b }; }
	constexpr Vector4 operator-(const Vector4& a, float b) noexcept { return { a.x - b, a.y - b, a.z - b, a.w - b }; }

	constexpr Vector2 operator-(float a, const Vector2& b) noexcept { return { a - b.x, a - b.y }; }
	constexpr Vector3 operator-(float a, const Vector3& b) noexcept { return { a - b.x, a - b.y, a - b.z }; }
	constexpr Vector4 operator-(float a, const Vector4& b) noexcept { return { a - b.x, a - b.y, a - b.z, a - b.w }; }

	constexpr Vector2 operator*(const Vector2& a, const Vector2& b) noexcept { return { a.x * b.x, a.y * b.y }; }
	constexpr Vector3 operator*(const Vector3& a, const Vector3& b) noexcept { return { a.x * b.x, a.y * b.y, a.z * b.z }; }
	constexpr Vector4 operator*(const Vector4& a, const Vector4& b) noexcept { return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; }

	constexpr Vector2 operator*(const Vector2& a, float b) noexcept { return { a.x * b, a.y * b }; }
	constexpr Vector3 operator*(const Vector3& a, float b) noexcept { return { a.x * b, a.y * b, a.z * b }; }
	constexpr Vector4 operator*(const Vector4& a, float b) noexcept { return { a.x * b, a.y * b, a.z * b, a.w * b }; }

	constexpr Vector2 operator*(float a, const Vector2& b) noexcept { return b * a; }
	constexpr Vector3 operator*(float a, const Vector3& b) noexcept { return b * a; }
	constexpr Vector4 operator*(float a, const Vector4& b) noexcept { return b * a; }

	constexpr Vector2 operator/(const Vector2& a, const Vector2& b) noexcept { return { a.x / b.x, a.y / b.y }; }
	constexpr Vector3 operator/(const Vector3& a, const Vector3& b) noexcept { return { a.x / b.x, a.y / b.y, a.z / b.z }; }
	constexpr Vector4 operator/(const Vector4& a, const Vector4& b) noexcept { return { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; }

	constexpr Vector2 operator/(const Vector2& a, float b) noexcept { return { a.x / b, a.y / b }; }
	constexpr Vector3 operator/(const Vector3& a, float b) noexcept { return { a.x / b, a.y / b, a.z / b }; }
	constexpr Vector4 operator/(const Vector4& a, float b) noexcept { return { a.x / b, a.y / b, a.z / b, a.w / b }; }

	constexpr Vector2 operator/(float a, const Vector2& b) noexcept { return { a / b.x, a / b.y }; }
	constexpr Vector3 operator/(float a, const Vector3& b) noexcept { return { a / b.x, a / b.y, a / b.z }; }
	constexpr Vector4 operator/(float a, const Vector4& b) noexcept { return { a / b.x, a / b.y, a / b.z, a / b.w }; }

	constexpr auto dot_product(const Vector3& a, const Vector3& b) noexcept
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline auto length(const Vector3& v) noexcept
	{
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	inline auto normalize(const Vector3& v) noexcept
	{
		return v / length(v);
	}
}
