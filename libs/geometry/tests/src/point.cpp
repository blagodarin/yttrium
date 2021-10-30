// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/geometry/point.h>

#include <doctest/doctest.h>

TEST_CASE("point")
{
	{
		Yt::Point p;
		CHECK(p._x == 0);
		CHECK(p._y == 0);
	}
	{
		Yt::Point p{ 1, 2 };
		CHECK(p._x == 1);
		CHECK(p._y == 2);
	}
}
