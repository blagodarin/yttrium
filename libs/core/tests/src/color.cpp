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

#include <yttrium/math/color.h>

#include <catch2/catch.hpp>

TEST_CASE("color.bgra32")
{
	{
		Yt::Bgra32 color{ 1, 2, 3 };
		CHECK(color._b == 1u);
		CHECK(color._g == 2u);
		CHECK(color._r == 3u);
		CHECK(color._a == 255u);
	}
	{
		Yt::Bgra32 color{ 1, 2, 3, 4 };
		CHECK(color._b == 1u);
		CHECK(color._g == 2u);
		CHECK(color._r == 3u);
		CHECK(color._a == 4u);
	}
	CHECK(Yt::Bgra32(1, 2, 3) == Yt::Bgra32(1, 2, 3, 0xff));
	CHECK(Yt::Bgra32(1, 2, 3, 4) != Yt::Bgra32(1, 2, 3, 0xff));
}
