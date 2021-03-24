// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/geometry/vector.h>

namespace Yt
{
	class Euler;
	class SizeF;

	class Matrix4
	{
	public:
		Vector4 x, y, z, t;

		Matrix4() noexcept = default;
		constexpr Matrix4(float xx, float yx, float zx, float tx, float xy, float yy, float zy, float ty, float xz, float yz, float zz, float tz, float xw, float yw, float zw, float tw) noexcept
			: x{ xx, xy, xz, xw }, y{ yx, yy, yz, yw }, z{ zx, zy, zz, zw }, t{ tx, ty, tz, tw } {}
		explicit Matrix4(const Euler&) noexcept;

		static Matrix4 camera(const Vector3& position, const Euler& orientation) noexcept;

		static constexpr Matrix4 identity() noexcept
		{
			return {
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};
		}

		static Matrix4 perspective(const SizeF&, float vertical_fov, float near_plane, float far_plane) noexcept;
		static Matrix4 projection_2d(const SizeF&, float near_plane = -1.f, float far_plane = 1.f) noexcept;

		static Matrix4 rotation(float degrees, const Vector3& axis) noexcept;

		static constexpr Matrix4 scaling(float s) noexcept
		{
			return {
				s, 0, 0, 0,
				0, s, 0, 0,
				0, 0, s, 0,
				0, 0, 0, 1
			};
		}

		static constexpr Matrix4 translation(const Vector3& v) noexcept
		{
			return {
				1, 0, 0, v.x,
				0, 1, 0, v.y,
				0, 0, 1, v.z,
				0, 0, 0, 1
			};
		}
	};

	constexpr Matrix4 operator*(const Matrix4& a, const Matrix4& b) noexcept
	{
		return {
			a.x.x * b.x.x + a.y.x * b.x.y + a.z.x * b.x.z + a.t.x * b.x.w,
			a.x.x * b.y.x + a.y.x * b.y.y + a.z.x * b.y.z + a.t.x * b.y.w,
			a.x.x * b.z.x + a.y.x * b.z.y + a.z.x * b.z.z + a.t.x * b.z.w,
			a.x.x * b.t.x + a.y.x * b.t.y + a.z.x * b.t.z + a.t.x * b.t.w,

			a.x.y * b.x.x + a.y.y * b.x.y + a.z.y * b.x.z + a.t.y * b.x.w,
			a.x.y * b.y.x + a.y.y * b.y.y + a.z.y * b.y.z + a.t.y * b.y.w,
			a.x.y * b.z.x + a.y.y * b.z.y + a.z.y * b.z.z + a.t.y * b.z.w,
			a.x.y * b.t.x + a.y.y * b.t.y + a.z.y * b.t.z + a.t.y * b.t.w,

			a.x.z * b.x.x + a.y.z * b.x.y + a.z.z * b.x.z + a.t.z * b.x.w,
			a.x.z * b.y.x + a.y.z * b.y.y + a.z.z * b.y.z + a.t.z * b.y.w,
			a.x.z * b.z.x + a.y.z * b.z.y + a.z.z * b.z.z + a.t.z * b.z.w,
			a.x.z * b.t.x + a.y.z * b.t.y + a.z.z * b.t.z + a.t.z * b.t.w,

			a.x.w * b.x.x + a.y.w * b.x.y + a.z.w * b.x.z + a.t.w * b.x.w,
			a.x.w * b.y.x + a.y.w * b.y.y + a.z.w * b.y.z + a.t.w * b.y.w,
			a.x.w * b.z.x + a.y.w * b.z.y + a.z.w * b.z.z + a.t.w * b.z.w,
			a.x.w * b.t.x + a.y.w * b.t.y + a.z.w * b.t.z + a.t.w * b.t.w,
		};
	}

	constexpr Vector4 operator*(const Matrix4& m, const Vector4& v) noexcept
	{
		return {
			m.x.x * v.x + m.y.x * v.y + m.z.x * v.z + m.t.x * v.w,
			m.x.y * v.x + m.y.y * v.y + m.z.y * v.z + m.t.y * v.w,
			m.x.z * v.x + m.y.z * v.y + m.z.z * v.z + m.t.z * v.w,
			m.x.w * v.x + m.y.w * v.y + m.z.w * v.z + m.t.w * v.w,
		};
	}

	constexpr Vector3 operator*(const Matrix4& m, const Vector3& v) noexcept
	{
		const auto w = m.x.w * v.x + m.y.w * v.y + m.z.w * v.z + m.t.w;
		return {
			(m.x.x * v.x + m.y.x * v.y + m.z.x * v.z + m.t.x) / w,
			(m.x.y * v.x + m.y.y * v.y + m.z.y * v.z + m.t.y) / w,
			(m.x.z * v.x + m.y.z * v.y + m.z.z * v.z + m.t.z) / w,
		};
	}

	float det(const Matrix4&) noexcept;
	Matrix4 inverse(const Matrix4&) noexcept;
}
