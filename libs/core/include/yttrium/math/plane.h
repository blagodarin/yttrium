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

#include <yttrium/math/vector.h>

namespace Yttrium
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
