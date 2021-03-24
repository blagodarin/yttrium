// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/geometry/vector.h>

namespace Yt
{
	class Plane
	{
	public:
		Vector3 _normal;
		float _offset;

		Plane() noexcept = default;
		Plane(const Vector3& normal, const Vector3& origin) noexcept
			: _normal{ normalize(normal) }, _offset{ dot_product(_normal, origin) } {}

		constexpr float distance_to(const Vector3& point) const noexcept { return dot_product(_normal, point) - _offset; }
	};
}
