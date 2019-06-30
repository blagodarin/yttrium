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

namespace Yttrium
{
	class Point
	{
	public:
		int _x = 0;
		int _y = 0;

		constexpr Point() noexcept = default;
		constexpr Point(int x, int y) noexcept
			: _x{ x }, _y{ y } {}
	};

	constexpr bool operator==(const Point& a, const Point& b) noexcept { return a._x == b._x && a._y == b._y; }
	constexpr bool operator!=(const Point& a, const Point& b) noexcept { return !(a == b); }
}
