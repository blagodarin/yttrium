#include <yttrium/math/iostream.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_rect_initialization)
{
	Rect null_rect;

	BOOST_CHECK_EQUAL(null_rect.left(), 0);
	BOOST_CHECK_EQUAL(null_rect.top(), 0);
	BOOST_CHECK_EQUAL(null_rect.right(), 0);
	BOOST_CHECK_EQUAL(null_rect.bottom(), 0);

	Rect rect({3, 5}, Size(7, 8));

	BOOST_CHECK_EQUAL(rect.left(), 3);
	BOOST_CHECK_EQUAL(rect.top(), 5);
	BOOST_CHECK_EQUAL(rect.width(), 7);
	BOOST_CHECK_EQUAL(rect.height(), 8);

	BOOST_CHECK_EQUAL(rect.left() + rect.width(), rect.right());
	BOOST_CHECK_EQUAL(rect.top() + rect.height(), rect.bottom());
}

BOOST_AUTO_TEST_CASE(test_rect_bound)
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

BOOST_AUTO_TEST_CASE(test_rect_center)
{
	BOOST_CHECK_EQUAL(Rect({1, 1}, Size(3, 3)).center(), Point(2, 2));
	BOOST_CHECK_EQUAL(Rect({2, 2}, Size(4, 4)).center(), Point(4, 4));
}

BOOST_AUTO_TEST_CASE(test_rect_centered_at)
{
	Rect rect1({0, 0}, Size(1, 1));
	Rect rect2({0, 0}, Size(3, 3));
	BOOST_CHECK_EQUAL(rect1.centered_at(rect2), Rect({1, 1}, Size(1, 1)));
	BOOST_CHECK_EQUAL(rect2.centered_at(rect1), Rect({-1, -1}, Size(3, 3)));
}

BOOST_AUTO_TEST_CASE(test_rect_contains)
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

BOOST_AUTO_TEST_CASE(test_rect_intersected)
{
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{0, 0}, Size(2, 2)}), Rect({0, 0}, Size(2, 2)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{0, 1}, Size(2, 2)}), Rect({0, 1}, Size(2, 1)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{1, 1}, Size(2, 2)}), Rect({1, 1}, Size(1, 1)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{0, 2}, Size(2, 2)}), Rect({0, 2}, Size(2, 0)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{1, 2}, Size(2, 2)}), Rect({1, 2}, Size(1, 0)));
	BOOST_CHECK_EQUAL(Rect({0, 0}, Size(2, 2)).intersected({{2, 2}, Size(2, 2)}), Rect({2, 2}, Size(0, 0)));
}

BOOST_AUTO_TEST_CASE(test_rect_intersects)
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

BOOST_AUTO_TEST_CASE(test_rect_intersects_null)
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

BOOST_AUTO_TEST_CASE(test_rect_ostream)
{
	std::ostringstream stream;
	stream << Rect{ { 1, 2 }, Size{ 3, 4 } };
	BOOST_CHECK_EQUAL(stream.str(), "Rect({1, 2}, Size(3, 4))");
}
