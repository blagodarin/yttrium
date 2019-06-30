//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/math/plane.h>

namespace Yttrium
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
