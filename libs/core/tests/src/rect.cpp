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

#include <yttrium/math/rect.h>
#include "iostream.h"

#include <doctest.h>

using Yt::Point;
using Yt::Rect;
using Yt::RectF;
using Yt::Size;
using Yt::SizeF;
using Yt::Vector2;

TEST_CASE("rect")
{
	{
		Rect r;
		CHECK(r.left() == 0);
		CHECK(r.top() == 0);
		CHECK(r.right() == 0);
		CHECK(r.bottom() == 0);
		CHECK(r.width() == 0);
		CHECK(r.height() == 0);
	}
	{
		Rect r{ { 1, 2 }, Point{ 4, 7 } };
		CHECK(r.left() == 1);
		CHECK(r.top() == 2);
		CHECK(r.right() == 4);
		CHECK(r.bottom() == 7);
		CHECK(r.width() == 3);
		CHECK(r.height() == 5);
	}
	{
		Rect r{ { 1, 2 }, Size{ 3, 5 } };
		CHECK(r.left() == 1);
		CHECK(r.top() == 2);
		CHECK(r.right() == 4);
		CHECK(r.bottom() == 7);
		CHECK(r.width() == 3);
		CHECK(r.height() == 5);
	}
	{
		Rect r{ Size{ 1, 2 } };
		CHECK(r.left() == 0);
		CHECK(r.top() == 0);
		CHECK(r.right() == 1);
		CHECK(r.bottom() == 2);
		CHECK(r.width() == 1);
		CHECK(r.height() == 2);
	}
}

TEST_CASE("rect.bound")
{
	// Points are treated like solid objects, so e.g. a rect of width 2
	// can only bound 2 different points with the same Y coordinate.

	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 0, 0 }) == Point(1, 1));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 1, 0 }) == Point(1, 1));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 2, 0 }) == Point(2, 1));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 4, 0 }) == Point(2, 1));

	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 0, 1 }) == Point(1, 1));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 1, 1 }) == Point(1, 1));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 2, 1 }) == Point(2, 1));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 4, 1 }) == Point(2, 1));

	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 0, 2 }) == Point(1, 2));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 1, 2 }) == Point(1, 2));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 2, 2 }) == Point(2, 2));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 4, 2 }) == Point(2, 2));

	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 0, 4 }) == Point(1, 2));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 1, 4 }) == Point(1, 2));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 2, 4 }) == Point(2, 2));
	CHECK(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 4, 4 }) == Point(2, 2));
}

TEST_CASE("rect.center")
{
	CHECK(Rect({ 1, 1 }, Size(3, 3)).center() == Point(2, 2));
	CHECK(Rect({ 2, 2 }, Size(4, 4)).center() == Point(4, 4));
}

TEST_CASE("rect.centered_at")
{
	Rect rect1({ 0, 0 }, Size(1, 1));
	Rect rect2({ 0, 0 }, Size(3, 3));
	CHECK(rect1.centered_at(rect2) == Rect({ 1, 1 }, Size(1, 1)));
	CHECK(rect2.centered_at(rect1) == Rect({ -1, -1 }, Size(3, 3)));
}

TEST_CASE("rect.contains")
{
	Rect rect({ 0, 0 }, Size(10, 10));

	CHECK(rect.contains(rect));

	CHECK(rect.contains({ 0, 0 }));
	CHECK(rect.contains_fast({ 0, 0 }));

	CHECK(!rect.contains({ -1, 0 }));
	CHECK(!rect.contains_fast({ -1, 0 }));

	CHECK(!rect.contains({ 0, -1 }));
	CHECK(!rect.contains_fast({ 0, -1 }));

	CHECK(rect.contains({ 9, 9 }));
	CHECK(rect.contains_fast({ 9, 9 }));

	CHECK(!rect.contains({ 10, 9 }));
	CHECK(!rect.contains_fast({ 10, 9 }));

	CHECK(!rect.contains({ 9, 10 }));
	CHECK(!rect.contains_fast({ 9, 10 }));

	Rect smaller_rect({ 1, 1 }, Size(9, 9));

	CHECK(rect.contains(smaller_rect));

	CHECK(!smaller_rect.contains(rect));

	smaller_rect = { { 0, 0 }, Size(9, 9) };

	CHECK(rect.contains(smaller_rect));

	CHECK(!smaller_rect.contains(rect));
}

TEST_CASE("rect.intersected")
{
	CHECK(Rect({ 0, 0 }, Size(2, 2)).intersected({ { 0, 0 }, Size(2, 2) }) == Rect({ 0, 0 }, Size(2, 2)));
	CHECK(Rect({ 0, 0 }, Size(2, 2)).intersected({ { 0, 1 }, Size(2, 2) }) == Rect({ 0, 1 }, Size(2, 1)));
	CHECK(Rect({ 0, 0 }, Size(2, 2)).intersected({ { 1, 1 }, Size(2, 2) }) == Rect({ 1, 1 }, Size(1, 1)));
	CHECK(Rect({ 0, 0 }, Size(2, 2)).intersected({ { 0, 2 }, Size(2, 2) }) == Rect({ 0, 2 }, Size(2, 0)));
	CHECK(Rect({ 0, 0 }, Size(2, 2)).intersected({ { 1, 2 }, Size(2, 2) }) == Rect({ 1, 2 }, Size(1, 0)));
	CHECK(Rect({ 0, 0 }, Size(2, 2)).intersected({ { 2, 2 }, Size(2, 2) }) == Rect({ 2, 2 }, Size(0, 0)));
}

TEST_CASE("rect.intersects")
{
	Rect rect({ 0, 0 }, Size(3, 3));

	CHECK(rect.intersects(rect));
	CHECK(rect.intersects_fast(rect));
	CHECK(rect.intersects_fastest(rect));

	Rect another_rect({ 1, 1 }, Size(3, 3));

	CHECK(rect.intersects(another_rect));
	CHECK(rect.intersects_fast(another_rect));
	CHECK(rect.intersects_fastest(another_rect));

	CHECK(another_rect.intersects(rect));
	CHECK(another_rect.intersects_fast(rect));
	CHECK(another_rect.intersects_fastest(rect));

	Rect inner_rect({ 1, 1 }, Size(1, 1)); // So that (L1 - R2 == L2 - R1).

	CHECK(rect.intersects(inner_rect));
	CHECK(rect.intersects_fast(inner_rect));
	CHECK(rect.intersects_fastest(inner_rect));

	CHECK(inner_rect.intersects(rect));
	CHECK(inner_rect.intersects_fast(rect));
	CHECK(inner_rect.intersects_fastest(rect));

	Rect right_border_rect({ 2, 0 }, Size(3, 3));

	CHECK(rect.intersects(right_border_rect));
	CHECK(rect.intersects_fast(right_border_rect));
	CHECK(rect.intersects_fastest(right_border_rect));

	CHECK(right_border_rect.intersects(rect));
	CHECK(right_border_rect.intersects_fast(rect));
	CHECK(right_border_rect.intersects_fastest(rect));

	Rect right_rect({ 3, 0 }, Size(3, 3));

	CHECK(!rect.intersects(right_rect));
	CHECK(!rect.intersects_fast(right_rect));
	CHECK(!rect.intersects_fastest(right_rect));

	CHECK(!right_rect.intersects(rect));
	CHECK(!right_rect.intersects_fast(rect));
	CHECK(!right_rect.intersects_fastest(rect));

	Rect bottom_border_rect({ 0, 2 }, Size(3, 3));

	CHECK(rect.intersects(bottom_border_rect));
	CHECK(rect.intersects_fast(bottom_border_rect));
	CHECK(rect.intersects_fastest(bottom_border_rect));

	CHECK(bottom_border_rect.intersects(rect));
	CHECK(bottom_border_rect.intersects_fast(rect));
	CHECK(bottom_border_rect.intersects_fastest(rect));

	Rect bottom_rect({ 0, 3 }, Size(3, 3));

	CHECK(!rect.intersects(bottom_rect));
	CHECK(!rect.intersects_fast(bottom_rect));
	CHECK(!rect.intersects_fastest(bottom_rect));

	CHECK(!bottom_rect.intersects(rect));
	CHECK(!bottom_rect.intersects_fast(rect));
	CHECK(!bottom_rect.intersects_fastest(rect));

	Rect far_rect({ 4, 4 }, Size(3, 3));

	CHECK(!rect.intersects(far_rect));
	CHECK(!rect.intersects_fast(far_rect));
	CHECK(!rect.intersects_fastest(far_rect));

	CHECK(!far_rect.intersects(rect));
	CHECK(!far_rect.intersects_fast(rect));
	CHECK(!far_rect.intersects_fastest(rect));
}

TEST_CASE("rect.intersects.null")
{
	// This test case exists only as an implementation reference,
	// one should not rely on null rect intersection detection!

	Rect null_rect;

	CHECK(!null_rect.intersects(null_rect));
	CHECK(!null_rect.intersects_fast(null_rect));
	CHECK(null_rect.intersects_fastest(null_rect)); // Note the difference.

	// A null rect intersects with a non-null rect if the top left coordinate of the
	// null rect lies within the non-null rect (and not on its zero-width border).
	// Note that a point is equivalent to the rect with width and height set to one, not zero!

	Rect rect({ 0, 0 }, Size(2, 2));

	CHECK(!null_rect.intersects(rect));
	CHECK(!null_rect.intersects_fast(rect));
	CHECK(!null_rect.intersects_fastest(rect));

	CHECK(!rect.intersects(null_rect));
	CHECK(!rect.intersects_fast(null_rect));
	CHECK(!rect.intersects_fastest(null_rect));

	null_rect = { { 1, 1 }, Size() };

	CHECK(null_rect.intersects(rect));
	CHECK(null_rect.intersects_fast(rect));
	CHECK(null_rect.intersects_fastest(rect));

	CHECK(rect.intersects(null_rect));
	CHECK(rect.intersects_fast(null_rect));
	CHECK(rect.intersects_fastest(null_rect));

	null_rect = { { 2, 2 }, Size() };

	CHECK(!null_rect.intersects(rect));
	CHECK(!null_rect.intersects_fast(rect));
	CHECK(!null_rect.intersects_fastest(rect));

	CHECK(!rect.intersects(null_rect));
	CHECK(!rect.intersects_fast(null_rect));
	CHECK(!rect.intersects_fastest(null_rect));
}

TEST_CASE("rectf")
{
	{
		RectF r;
		CHECK(r.left() == 0.f);
		CHECK(r.top() == 0.f);
		CHECK(r.right() == 0.f);
		CHECK(r.bottom() == 0.f);
		CHECK(r.width() == 0.f);
		CHECK(r.height() == 0.f);
	}
	{
		RectF r{ { 1, 2 }, Vector2{ 4, 7 } };
		CHECK(r.left() == 1.f);
		CHECK(r.top() == 2.f);
		CHECK(r.right() == 4.f);
		CHECK(r.bottom() == 7.f);
		CHECK(r.width() == 3.f);
		CHECK(r.height() == 5.f);
	}
	{
		RectF r{ { 1, 2 }, SizeF{ 3, 5 } };
		CHECK(r.left() == 1.f);
		CHECK(r.top() == 2.f);
		CHECK(r.right() == 4.f);
		CHECK(r.bottom() == 7.f);
		CHECK(r.width() == 3.f);
		CHECK(r.height() == 5.f);
	}
	{
		RectF r{ SizeF{ 1, 2 } };
		CHECK(r.left() == 0.f);
		CHECK(r.top() == 0.f);
		CHECK(r.right() == 1.f);
		CHECK(r.bottom() == 2.f);
		CHECK(r.width() == 1.f);
		CHECK(r.height() == 2.f);
	}
	{
		RectF r{ Rect{ { 1, 2 }, Point{ 4, 7 } } };
		CHECK(r.left() == 1.f);
		CHECK(r.top() == 2.f);
		CHECK(r.right() == 4.f);
		CHECK(r.bottom() == 7.f);
		CHECK(r.width() == 3.f);
		CHECK(r.height() == 5.f);
	}
}
