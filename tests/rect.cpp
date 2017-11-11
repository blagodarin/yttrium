#include "iostream.h"

#include <boost/test/unit_test.hpp>

using Yttrium::Point;
using Yttrium::Rect;
using Yttrium::RectF;
using Yttrium::Size;
using Yttrium::SizeF;
using Yttrium::Vector2;

BOOST_AUTO_TEST_CASE(rect_construction)
{
	{
		Rect r;
		BOOST_CHECK_EQUAL(r.left(), 0);
		BOOST_CHECK_EQUAL(r.top(), 0);
		BOOST_CHECK_EQUAL(r.right(), 0);
		BOOST_CHECK_EQUAL(r.bottom(), 0);
		BOOST_CHECK_EQUAL(r.width(), 0);
		BOOST_CHECK_EQUAL(r.height(), 0);
	}
	{
		Rect r{{1, 2}, Point{4, 7}};
		BOOST_CHECK_EQUAL(r.left(), 1);
		BOOST_CHECK_EQUAL(r.top(), 2);
		BOOST_CHECK_EQUAL(r.right(), 4);
		BOOST_CHECK_EQUAL(r.bottom(), 7);
		BOOST_CHECK_EQUAL(r.width(), 3);
		BOOST_CHECK_EQUAL(r.height(), 5);
	}
	{
		Rect r{{1, 2}, Size{3, 5}};
		BOOST_CHECK_EQUAL(r.left(), 1);
		BOOST_CHECK_EQUAL(r.top(), 2);
		BOOST_CHECK_EQUAL(r.right(), 4);
		BOOST_CHECK_EQUAL(r.bottom(), 7);
		BOOST_CHECK_EQUAL(r.width(), 3);
		BOOST_CHECK_EQUAL(r.height(), 5);
	}
	{
		Rect r{Size{1, 2}};
		BOOST_CHECK_EQUAL(r.left(), 0);
		BOOST_CHECK_EQUAL(r.top(), 0);
		BOOST_CHECK_EQUAL(r.right(), 1);
		BOOST_CHECK_EQUAL(r.bottom(), 2);
		BOOST_CHECK_EQUAL(r.width(), 1);
		BOOST_CHECK_EQUAL(r.height(), 2);
	}
}

BOOST_AUTO_TEST_CASE(rect_bound)
{
	// Points are treated like solid objects, so e.g. a rect of width 2
	// can only bound 2 different points with the same Y coordinate.

	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 0, 0 }), Point(1, 1));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 1, 0 }), Point(1, 1));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 2, 0 }), Point(2, 1));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 4, 0 }), Point(2, 1));

	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 0, 1 }), Point(1, 1));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 1, 1 }), Point(1, 1));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 2, 1 }), Point(2, 1));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 4, 1 }), Point(2, 1));

	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 0, 2 }), Point(1, 2));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 1, 2 }), Point(1, 2));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 2, 2 }), Point(2, 2));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 4, 2 }), Point(2, 2));

	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 0, 4 }), Point(1, 2));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 1, 4 }), Point(1, 2));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 2, 4 }), Point(2, 2));
	BOOST_CHECK_EQUAL(Rect({ 1, 1 }, Size{ 2, 2 }).bound({ 4, 4 }), Point(2, 2));
}

BOOST_AUTO_TEST_CASE(rect_center)
{
	BOOST_CHECK_EQUAL(Rect({1, 1}, Size(3, 3)).center(), Point(2, 2));
	BOOST_CHECK_EQUAL(Rect({2, 2}, Size(4, 4)).center(), Point(4, 4));
}

BOOST_AUTO_TEST_CASE(rect_centered_at)
{
	Rect rect1({0, 0}, Size(1, 1));
	Rect rect2({0, 0}, Size(3, 3));
	BOOST_CHECK_EQUAL(rect1.centered_at(rect2), Rect({1, 1}, Size(1, 1)));
	BOOST_CHECK_EQUAL(rect2.centered_at(rect1), Rect({-1, -1}, Size(3, 3)));
}

BOOST_AUTO_TEST_CASE(rect_contains)
{
	Rect rect({0, 0}, Size(10, 10));

	BOOST_CHECK(rect.contains(rect));

	BOOST_CHECK(rect.contains({0, 0}));
	BOOST_CHECK(rect.contains_fast({0, 0}));

	BOOST_CHECK(!rect.contains({-1, 0}));
	BOOST_CHECK(!rect.contains_fast({-1, 0}));

	BOOST_CHECK(!rect.contains({0, -1}));
	BOOST_CHECK(!rect.contains_fast({0, -1}));

	BOOST_CHECK(rect.contains({9, 9}));
	BOOST_CHECK(rect.contains_fast({9, 9}));

	BOOST_CHECK(!rect.contains({10, 9}));
	BOOST_CHECK(!rect.contains_fast({10, 9}));

	BOOST_CHECK(!rect.contains({9, 10}));
	BOOST_CHECK(!rect.contains_fast({9, 10}));

	Rect smaller_rect({1, 1}, Size(9, 9));

	BOOST_CHECK(rect.contains(smaller_rect));

	BOOST_CHECK(!smaller_rect.contains(rect));

	smaller_rect = {{0, 0}, Size(9, 9)};

	BOOST_CHECK(rect.contains(smaller_rect));

	BOOST_CHECK(!smaller_rect.contains(rect));
}

BOOST_AUTO_TEST_CASE(rect_intersected)
{
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{0, 0}, Size(2, 2)}), Rect({0, 0}, Size(2, 2)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{0, 1}, Size(2, 2)}), Rect({0, 1}, Size(2, 1)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{1, 1}, Size(2, 2)}), Rect({1, 1}, Size(1, 1)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{0, 2}, Size(2, 2)}), Rect({0, 2}, Size(2, 0)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{1, 2}, Size(2, 2)}), Rect({1, 2}, Size(1, 0)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{2, 2}, Size(2, 2)}), Rect({2, 2}, Size(0, 0)));
}

BOOST_AUTO_TEST_CASE(rect_intersects)
{
	Rect rect({0, 0}, Size(3, 3));

	BOOST_CHECK(rect.intersects(rect));
	BOOST_CHECK(rect.intersects_fast(rect));
	BOOST_CHECK(rect.intersects_fastest(rect));

	Rect another_rect({1, 1}, Size(3, 3));

	BOOST_CHECK(rect.intersects(another_rect));
	BOOST_CHECK(rect.intersects_fast(another_rect));
	BOOST_CHECK(rect.intersects_fastest(another_rect));

	BOOST_CHECK(another_rect.intersects(rect));
	BOOST_CHECK(another_rect.intersects_fast(rect));
	BOOST_CHECK(another_rect.intersects_fastest(rect));

	Rect inner_rect({1, 1}, Size(1, 1)); // So that (L1 - R2 == L2 - R1).

	BOOST_CHECK(rect.intersects(inner_rect));
	BOOST_CHECK(rect.intersects_fast(inner_rect));
	BOOST_CHECK(rect.intersects_fastest(inner_rect));

	BOOST_CHECK(inner_rect.intersects(rect));
	BOOST_CHECK(inner_rect.intersects_fast(rect));
	BOOST_CHECK(inner_rect.intersects_fastest(rect));

	Rect right_border_rect({2, 0}, Size(3, 3));

	BOOST_CHECK(rect.intersects(right_border_rect));
	BOOST_CHECK(rect.intersects_fast(right_border_rect));
	BOOST_CHECK(rect.intersects_fastest(right_border_rect));

	BOOST_CHECK(right_border_rect.intersects(rect));
	BOOST_CHECK(right_border_rect.intersects_fast(rect));
	BOOST_CHECK(right_border_rect.intersects_fastest(rect));

	Rect right_rect({3, 0}, Size(3, 3));

	BOOST_CHECK(!rect.intersects(right_rect));
	BOOST_CHECK(!rect.intersects_fast(right_rect));
	BOOST_CHECK(!rect.intersects_fastest(right_rect));

	BOOST_CHECK(!right_rect.intersects(rect));
	BOOST_CHECK(!right_rect.intersects_fast(rect));
	BOOST_CHECK(!right_rect.intersects_fastest(rect));

	Rect bottom_border_rect({0, 2}, Size(3, 3));

	BOOST_CHECK(rect.intersects(bottom_border_rect));
	BOOST_CHECK(rect.intersects_fast(bottom_border_rect));
	BOOST_CHECK(rect.intersects_fastest(bottom_border_rect));

	BOOST_CHECK(bottom_border_rect.intersects(rect));
	BOOST_CHECK(bottom_border_rect.intersects_fast(rect));
	BOOST_CHECK(bottom_border_rect.intersects_fastest(rect));

	Rect bottom_rect({0, 3}, Size(3, 3));

	BOOST_CHECK(!rect.intersects(bottom_rect));
	BOOST_CHECK(!rect.intersects_fast(bottom_rect));
	BOOST_CHECK(!rect.intersects_fastest(bottom_rect));

	BOOST_CHECK(!bottom_rect.intersects(rect));
	BOOST_CHECK(!bottom_rect.intersects_fast(rect));
	BOOST_CHECK(!bottom_rect.intersects_fastest(rect));

	Rect far_rect({4, 4}, Size(3, 3));

	BOOST_CHECK(!rect.intersects(far_rect));
	BOOST_CHECK(!rect.intersects_fast(far_rect));
	BOOST_CHECK(!rect.intersects_fastest(far_rect));

	BOOST_CHECK(!far_rect.intersects(rect));
	BOOST_CHECK(!far_rect.intersects_fast(rect));
	BOOST_CHECK(!far_rect.intersects_fastest(rect));
}

BOOST_AUTO_TEST_CASE(rect_intersects_null)
{
	// This test case exists only as an implementation reference,
	// one should not rely on null rect intersection detection!

	Rect null_rect;

	BOOST_CHECK(!null_rect.intersects(null_rect));
	BOOST_CHECK(!null_rect.intersects_fast(null_rect));
	BOOST_CHECK(null_rect.intersects_fastest(null_rect)); // Note the difference.

	// A null rect intersects with a non-null rect if the top left coordinate of the
	// null rect lies within the non-null rect (and not on its zero-width border).
	// Note that a point is equivalent to the rect with width and height set to one, not zero!

	Rect rect({0, 0}, Size(2, 2));

	BOOST_CHECK(!null_rect.intersects(rect));
	BOOST_CHECK(!null_rect.intersects_fast(rect));
	BOOST_CHECK(!null_rect.intersects_fastest(rect));

	BOOST_CHECK(!rect.intersects(null_rect));
	BOOST_CHECK(!rect.intersects_fast(null_rect));
	BOOST_CHECK(!rect.intersects_fastest(null_rect));

	null_rect = {{1, 1}, Size()};

	BOOST_CHECK(null_rect.intersects(rect));
	BOOST_CHECK(null_rect.intersects_fast(rect));
	BOOST_CHECK(null_rect.intersects_fastest(rect));

	BOOST_CHECK(rect.intersects(null_rect));
	BOOST_CHECK(rect.intersects_fast(null_rect));
	BOOST_CHECK(rect.intersects_fastest(null_rect));

	null_rect = {{2, 2}, Size()};

	BOOST_CHECK(!null_rect.intersects(rect));
	BOOST_CHECK(!null_rect.intersects_fast(rect));
	BOOST_CHECK(!null_rect.intersects_fastest(rect));

	BOOST_CHECK(!rect.intersects(null_rect));
	BOOST_CHECK(!rect.intersects_fast(null_rect));
	BOOST_CHECK(!rect.intersects_fastest(null_rect));
}

BOOST_AUTO_TEST_CASE(rectf_construction)
{
	{
		RectF r;
		BOOST_CHECK_EQUAL(r.left(), 0.f);
		BOOST_CHECK_EQUAL(r.top(), 0.f);
		BOOST_CHECK_EQUAL(r.right(), 0.f);
		BOOST_CHECK_EQUAL(r.bottom(), 0.f);
		BOOST_CHECK_EQUAL(r.width(), 0.f);
		BOOST_CHECK_EQUAL(r.height(), 0.f);
	}
	{
		RectF r{{1, 2}, Vector2{4, 7}};
		BOOST_CHECK_EQUAL(r.left(), 1.f);
		BOOST_CHECK_EQUAL(r.top(), 2.f);
		BOOST_CHECK_EQUAL(r.right(), 4.f);
		BOOST_CHECK_EQUAL(r.bottom(), 7.f);
		BOOST_CHECK_EQUAL(r.width(), 3.f);
		BOOST_CHECK_EQUAL(r.height(), 5.f);
	}
	{
		RectF r{{1, 2}, SizeF{3, 5}};
		BOOST_CHECK_EQUAL(r.left(), 1.f);
		BOOST_CHECK_EQUAL(r.top(), 2.f);
		BOOST_CHECK_EQUAL(r.right(), 4.f);
		BOOST_CHECK_EQUAL(r.bottom(), 7.f);
		BOOST_CHECK_EQUAL(r.width(), 3.f);
		BOOST_CHECK_EQUAL(r.height(), 5.f);
	}
	{
		RectF r{SizeF{1, 2}};
		BOOST_CHECK_EQUAL(r.left(), 0.f);
		BOOST_CHECK_EQUAL(r.top(), 0.f);
		BOOST_CHECK_EQUAL(r.right(), 1.f);
		BOOST_CHECK_EQUAL(r.bottom(), 2.f);
		BOOST_CHECK_EQUAL(r.width(), 1.f);
		BOOST_CHECK_EQUAL(r.height(), 2.f);
	}
	{
		RectF r{Rect{{1, 2}, Point{4, 7}}};
		BOOST_CHECK_EQUAL(r.left(), 1.f);
		BOOST_CHECK_EQUAL(r.top(), 2.f);
		BOOST_CHECK_EQUAL(r.right(), 4.f);
		BOOST_CHECK_EQUAL(r.bottom(), 7.f);
		BOOST_CHECK_EQUAL(r.width(), 3.f);
		BOOST_CHECK_EQUAL(r.height(), 5.f);
	}
}
