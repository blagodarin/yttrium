// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/geometry/plane.h>

namespace Yt
{
	class Line3
	{
	public:
		Vector3 _origin;
		Vector3 _vector;

		Line3() noexcept = default;
		constexpr Line3(const Vector3& from, const Vector3& to) noexcept
			: _origin{ from }, _vector{ to - from } {}

		bool plane_intersection(const Plane& plane, Vector3& intersection) const noexcept
		{
			const auto s = dot_product(_vector, plane._normal);
			if (std::abs(s) < 1e-6f)
				return false;
			const auto u = plane.distance_to(_origin) / -s;
			if (u < 0 || u > 1)
				return false;
			intersection = _origin + u * _vector;
			return true;
		}
	};
}
