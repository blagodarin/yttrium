// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/geometry/margins.h>

#include <doctest/doctest.h>

TEST_CASE("margins")
{
	{
		Yt::Margins m;
		CHECK(m._left == 0);
		CHECK(m._top == 0);
		CHECK(m._right == 0);
		CHECK(m._bottom == 0);
	}
	{
		Yt::Margins m{ 1 };
		CHECK(m._left == 1);
		CHECK(m._top == 1);
		CHECK(m._right == 1);
		CHECK(m._bottom == 1);
	}
	{
		Yt::Margins m{ 1, 2 };
		CHECK(m._left == 2);
		CHECK(m._top == 1);
		CHECK(m._right == 2);
		CHECK(m._bottom == 1);
	}
	{
		Yt::Margins m{ 1, 2, 3 };
		CHECK(m._left == 2);
		CHECK(m._top == 1);
		CHECK(m._right == 2);
		CHECK(m._bottom == 3);
	}
	{
		Yt::Margins m{ 1, 2, 3, 4 };
		CHECK(m._left == 4);
		CHECK(m._top == 1);
		CHECK(m._right == 2);
		CHECK(m._bottom == 3);
	}
}

TEST_CASE("marginsf")
{
	{
		Yt::MarginsF m;
		CHECK(m._left == 0.f);
		CHECK(m._top == 0.f);
		CHECK(m._right == 0.f);
		CHECK(m._bottom == 0.f);
	}
	{
		Yt::MarginsF m{ 1.f };
		CHECK(m._left == 1.f);
		CHECK(m._top == 1.f);
		CHECK(m._right == 1.f);
		CHECK(m._bottom == 1.f);
	}
	{
		Yt::MarginsF m{ 1.f, 2.f };
		CHECK(m._left == 2.f);
		CHECK(m._top == 1.f);
		CHECK(m._right == 2.f);
		CHECK(m._bottom == 1.f);
	}
	{
		Yt::MarginsF m{ 1.f, 2.f, 3.f };
		CHECK(m._left == 2.f);
		CHECK(m._top == 1.f);
		CHECK(m._right == 2.f);
		CHECK(m._bottom == 3.f);
	}
	{
		Yt::MarginsF m{ 1.f, 2.f, 3.f, 4.f };
		CHECK(m._left == 4.f);
		CHECK(m._top == 1.f);
		CHECK(m._right == 2.f);
		CHECK(m._bottom == 3.f);
	}
}
