// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/geometry/matrix.h>

#include <yttrium/geometry/euler.h>
#include <yttrium/geometry/size.h>

#include <cmath>
#include <numbers>

namespace Yt
{
	Matrix4::Matrix4(const Euler& e) noexcept
	{
		const auto yaw = e._yaw / 180 * std::numbers::pi_v<float>;
		const auto pitch = e._pitch / 180 * std::numbers::pi_v<float>;
		const auto roll = e._roll / 180 * std::numbers::pi_v<float>;
		const auto cy = std::cos(yaw);
		const auto sy = std::sin(yaw);
		const auto cp = std::cos(pitch);
		const auto sp = std::sin(pitch);
		const auto cr = std::cos(roll);
		const auto sr = std::sin(roll);
		x = { sy * sp * sr + cy * cr, cy * sp * sr - sy * cr, -cp * sr, 0 };
		y = { sy * cp, cy * cp, sp, 0 };
		z = { cy * sr - sy * sp * cr, -cy * sp * cr - sy * sr, cp * cr, 0 };
		t = { 0, 0, 0, 1 };
	}

	Matrix4 Matrix4::camera(const Vector3& position, const Euler& orientation) noexcept
	{
		const Matrix4 r{ orientation };
		return {
			r.x.x, r.x.y, r.x.z, -dot_product(position, { r.x.x, r.x.y, r.x.z }),
			r.y.x, r.y.y, r.y.z, -dot_product(position, { r.y.x, r.y.y, r.y.z }),
			r.z.x, r.z.y, r.z.z, -dot_product(position, { r.z.x, r.z.y, r.z.z }),
			0, 0, 0, 1
		};
	}

	Matrix4 Matrix4::perspective(const SizeF& size, float vertical_fov, float near_plane, float far_plane) noexcept
	{
		const auto aspect = size._width / size._height;
		const auto f = 1 / std::tan(vertical_fov / 360 * std::numbers::pi_v<float>);
		const auto xx = f / aspect;
		const auto yy = f;
		const auto zz = (near_plane + far_plane) / (near_plane - far_plane);
		const auto tz = 2 * near_plane * far_plane / (near_plane - far_plane);
		const auto zw = -1.f;
		return {
			xx, 0, 0, 0,
			0, yy, 0, 0,
			0, 0, zz, tz,
			0, 0, zw, 0
		};
	}

	Matrix4 Matrix4::projection_2d(const SizeF& size, float near_plane, float far_plane) noexcept
	{
		const auto xx = 2 / size._width;
		const auto yy = -2 / size._height;
		const auto zz = -2 / (far_plane - near_plane);
		const auto tx = -1.f;
		const auto ty = 1.f;
		const auto tz = (far_plane + near_plane) / (far_plane - near_plane);
		return {
			xx, 0, 0, tx,
			0, yy, 0, ty,
			0, 0, zz, tz,
			0, 0, 0, 1
		};
	}

	Matrix4 Matrix4::rotation(float degrees, const Vector3& axis) noexcept
	{
		const auto v = normalize(axis);
		const auto radians = degrees / 180 * std::numbers::pi_v<float>;
		const auto c = std::cos(radians);
		const auto s = std::sin(radians);
		return {
			v.x * v.x * (1 - c) + c, v.y * v.x * (1 - c) - s * v.z, v.z * v.x * (1 - c) + s * v.y, 0,
			v.x * v.y * (1 - c) + s * v.z, v.y * v.y * (1 - c) + c, v.z * v.y * (1 - c) - s * v.x, 0,
			v.x * v.z * (1 - c) - s * v.y, v.y * v.z * (1 - c) + s * v.x, v.z * v.z * (1 - c) + c, 0,
			0, 0, 0, 1
		};
	}

	float det(const Matrix4& m) noexcept
	{
		const auto xy = m.x.z * m.y.w - m.x.w * m.y.z;
		const auto xz = m.x.z * m.z.w - m.x.w * m.z.z;
		const auto xt = m.x.z * m.t.w - m.x.w * m.t.z;
		const auto yz = m.y.z * m.z.w - m.y.w * m.z.z;
		const auto yt = m.y.z * m.t.w - m.y.w * m.t.z;
		const auto zt = m.z.z * m.t.w - m.z.w * m.t.z;

		const auto yzt = m.y.y * zt - m.z.y * yt + m.t.y * yz;
		const auto xzt = m.x.y * zt - m.z.y * xt + m.t.y * xz;
		const auto xyt = m.x.y * yt - m.y.y * xt + m.t.y * xy;
		const auto xyz = m.x.y * yz - m.y.y * xz + m.z.y * xy;

		return m.x.x * yzt - m.y.x * xzt + m.z.x * xyt - m.t.x * xyz;
	}

	Matrix4 inverse(const Matrix4& m) noexcept
	{
		// Z and W rows.
		auto det01 = m.x.z * m.y.w - m.x.w * m.y.z;
		auto det02 = m.x.z * m.z.w - m.x.w * m.z.z;
		auto det03 = m.x.z * m.t.w - m.x.w * m.t.z;
		auto det12 = m.y.z * m.z.w - m.y.w * m.z.z;
		auto det13 = m.y.z * m.t.w - m.y.w * m.t.z;
		auto det23 = m.z.z * m.t.w - m.z.w * m.t.z;

		// Y, Z and W rows.
		const auto det123 = m.y.y * det23 - m.z.y * det13 + m.t.y * det12;
		const auto det023 = m.x.y * det23 - m.z.y * det03 + m.t.y * det02;
		const auto det013 = m.x.y * det13 - m.y.y * det03 + m.t.y * det01;
		const auto det012 = m.x.y * det12 - m.y.y * det02 + m.z.y * det01;

		const auto d = 1 / (m.x.x * det123 - m.y.x * det023 + m.z.x * det013 - m.t.x * det012);

		const auto xx = d * det123;
		const auto xy = d * -det023;
		const auto xz = d * det013;
		const auto xw = d * -det012;

		const auto yx = d * -(m.y.x * det23 - m.z.x * det13 + m.t.x * det12);
		const auto yy = d * (m.x.x * det23 - m.z.x * det03 + m.t.x * det02);
		const auto yz = d * -(m.x.x * det13 - m.y.x * det03 + m.t.x * det01);
		const auto yw = d * (m.x.x * det12 - m.y.x * det02 + m.z.x * det01);

		// Y and W rows.
		det01 = m.x.y * m.y.w - m.y.y * m.x.w;
		det02 = m.x.y * m.z.w - m.z.y * m.x.w;
		det03 = m.x.y * m.t.w - m.t.y * m.x.w;
		det12 = m.y.y * m.z.w - m.z.y * m.y.w;
		det13 = m.y.y * m.t.w - m.t.y * m.y.w;
		det23 = m.z.y * m.t.w - m.t.y * m.z.w;

		const auto zx = d * (m.y.x * det23 - m.z.x * det13 + m.t.x * det12);
		const auto zy = d * -(m.x.x * det23 - m.z.x * det03 + m.t.x * det02);
		const auto zz = d * (m.x.x * det13 - m.y.x * det03 + m.t.x * det01);
		const auto zw = d * -(m.x.x * det12 - m.y.x * det02 + m.z.x * det01);

		// Y and Z rows.
		det01 = m.y.z * m.x.y - m.x.z * m.y.y;
		det02 = m.z.z * m.x.y - m.x.z * m.z.y;
		det03 = m.t.z * m.x.y - m.x.z * m.t.y;
		det12 = m.z.z * m.y.y - m.y.z * m.z.y;
		det13 = m.t.z * m.y.y - m.y.z * m.t.y;
		det23 = m.t.z * m.z.y - m.z.z * m.t.y;

		const auto tx = d * -(m.y.x * det23 - m.z.x * det13 + m.t.x * det12);
		const auto ty = d * (m.x.x * det23 - m.z.x * det03 + m.t.x * det02);
		const auto tz = d * -(m.x.x * det13 - m.y.x * det03 + m.t.x * det01);
		const auto tw = d * (m.x.x * det12 - m.y.x * det02 + m.z.x * det01);

		return {
			xx, yx, zx, tx,
			xy, yy, zy, ty,
			xz, yz, zz, tz,
			xw, yw, zw, tw
		};
	}
}
