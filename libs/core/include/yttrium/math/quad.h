//
// Copyright 2019 Sergei Blagodarin
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
	class Quad
	{
	public:
		Vector2 _a, _b, _c, _d;

		Quad() noexcept = default;
		constexpr Quad(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d) noexcept
			: _a{ a }, _b{ b }, _c{ c }, _d{ d } {}
	};
}
