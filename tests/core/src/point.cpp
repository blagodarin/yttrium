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

#include <yttrium/math/point.h>

#include <catch2/catch.hpp>

TEST_CASE("point")
{
	using Yttrium::Point;
	{
		Point p;
		CHECK(p._x == 0);
		CHECK(p._y == 0);
	}
	{
		Point p{ 1, 2 };
		CHECK(p._x == 1);
		CHECK(p._y == 2);
	}
}
