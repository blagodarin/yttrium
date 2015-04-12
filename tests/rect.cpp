#include <yttrium/rect.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_rect_initialization)
{
	Rect null_rect;

	BOOST_CHECK_EQUAL(null_rect.left(), 0);
	BOOST_CHECK_EQUAL(null_rect.top(), 0);
	BOOST_CHECK_EQUAL(null_rect.right(), 0);
	BOOST_CHECK_EQUAL(null_rect.bottom(), 0);

	Rect rect(3, 5, 7, 8);

	BOOST_CHECK_EQUAL(rect.left(), 3);
	BOOST_CHECK_EQUAL(rect.top(), 5);
	BOOST_CHECK_EQUAL(rect.width(), 7);
	BOOST_CHECK_EQUAL(rect.height(), 8);

	BOOST_CHECK_EQUAL(rect.left() + rect.width(), rect.right());
	BOOST_CHECK_EQUAL(rect.top() + rect.height(), rect.bottom());
}

BOOST_AUTO_TEST_CASE(test_rect_containment)
{
	Rect rect(0, 0, 10, 10);

	BOOST_CHECK(rect.contains(rect));

	BOOST_CHECK(rect.contains(Point(0, 0)));
	BOOST_CHECK(rect.contains_fast(Point(0, 0)));

	BOOST_CHECK(!rect.contains(Point(-1, 0)));
	BOOST_CHECK(!rect.contains_fast(Point(-1, 0)));

	BOOST_CHECK(!rect.contains(Point(0, -1)));
	BOOST_CHECK(!rect.contains_fast(Point(0, -1)));

	BOOST_CHECK(rect.contains(Point(9, 9)));
	BOOST_CHECK(rect.contains_fast(Point(9, 9)));

	BOOST_CHECK(!rect.contains(Point(10, 9)));
	BOOST_CHECK(!rect.contains_fast(Point(10, 9)));

	BOOST_CHECK(!rect.contains(Point(9, 10)));
	BOOST_CHECK(!rect.contains_fast(Point(9, 10)));

	Rect smaller_rect(1, 1, 9, 9);

	BOOST_CHECK(rect.contains(smaller_rect));

	BOOST_CHECK(!smaller_rect.contains(rect));

	smaller_rect = Rect(0, 0, 9, 9);

	BOOST_CHECK(rect.contains(smaller_rect));

	BOOST_CHECK(!smaller_rect.contains(rect));
}

BOOST_AUTO_TEST_CASE(test_rect_intersection)
{
	Rect rect(0, 0, 3, 3);

	BOOST_CHECK(rect.intersects(rect));
	BOOST_CHECK(rect.intersects_fast(rect));
	BOOST_CHECK(rect.intersects_fastest(rect));

	Rect another_rect(1, 1, 3, 3);

	BOOST_CHECK(rect.intersects(another_rect));
	BOOST_CHECK(rect.intersects_fast(another_rect));
	BOOST_CHECK(rect.intersects_fastest(another_rect));

	BOOST_CHECK(another_rect.intersects(rect));
	BOOST_CHECK(another_rect.intersects_fast(rect));
	BOOST_CHECK(another_rect.intersects_fastest(rect));

	Rect inner_rect(1, 1, 1, 1); // So that (L1 - R2 == L2 - R1).

	BOOST_CHECK(rect.intersects(inner_rect));
	BOOST_CHECK(rect.intersects_fast(inner_rect));
	BOOST_CHECK(rect.intersects_fastest(inner_rect));

	BOOST_CHECK(inner_rect.intersects(rect));
	BOOST_CHECK(inner_rect.intersects_fast(rect));
	BOOST_CHECK(inner_rect.intersects_fastest(rect));

	Rect right_border_rect(2, 0, 3, 3);

	BOOST_CHECK(rect.intersects(right_border_rect));
	BOOST_CHECK(rect.intersects_fast(right_border_rect));
	BOOST_CHECK(rect.intersects_fastest(right_border_rect));

	BOOST_CHECK(right_border_rect.intersects(rect));
	BOOST_CHECK(right_border_rect.intersects_fast(rect));
	BOOST_CHECK(right_border_rect.intersects_fastest(rect));

	Rect right_rect(3, 0, 3, 3);

	BOOST_CHECK(!rect.intersects(right_rect));
	BOOST_CHECK(!rect.intersects_fast(right_rect));
	BOOST_CHECK(!rect.intersects_fastest(right_rect));

	BOOST_CHECK(!right_rect.intersects(rect));
	BOOST_CHECK(!right_rect.intersects_fast(rect));
	BOOST_CHECK(!right_rect.intersects_fastest(rect));

	Rect bottom_border_rect(0, 2, 3, 3);

	BOOST_CHECK(rect.intersects(bottom_border_rect));
	BOOST_CHECK(rect.intersects_fast(bottom_border_rect));
	BOOST_CHECK(rect.intersects_fastest(bottom_border_rect));

	BOOST_CHECK(bottom_border_rect.intersects(rect));
	BOOST_CHECK(bottom_border_rect.intersects_fast(rect));
	BOOST_CHECK(bottom_border_rect.intersects_fastest(rect));

	Rect bottom_rect(0, 3, 3, 3);

	BOOST_CHECK(!rect.intersects(bottom_rect));
	BOOST_CHECK(!rect.intersects_fast(bottom_rect));
	BOOST_CHECK(!rect.intersects_fastest(bottom_rect));

	BOOST_CHECK(!bottom_rect.intersects(rect));
	BOOST_CHECK(!bottom_rect.intersects_fast(rect));
	BOOST_CHECK(!bottom_rect.intersects_fastest(rect));

	Rect far_rect(4, 4, 3, 3);

	BOOST_CHECK(!rect.intersects(far_rect));
	BOOST_CHECK(!rect.intersects_fast(far_rect));
	BOOST_CHECK(!rect.intersects_fastest(far_rect));

	BOOST_CHECK(!far_rect.intersects(rect));
	BOOST_CHECK(!far_rect.intersects_fast(rect));
	BOOST_CHECK(!far_rect.intersects_fastest(rect));
}

BOOST_AUTO_TEST_CASE(test_rect_null_intersection)
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

	Rect rect(0, 0, 2, 2);

	BOOST_CHECK(!null_rect.intersects(rect));
	BOOST_CHECK(!null_rect.intersects_fast(rect));
	BOOST_CHECK(!null_rect.intersects_fastest(rect));

	BOOST_CHECK(!rect.intersects(null_rect));
	BOOST_CHECK(!rect.intersects_fast(null_rect));
	BOOST_CHECK(!rect.intersects_fastest(null_rect));

	null_rect = Rect(1, 1, 0, 0);

	BOOST_CHECK(null_rect.intersects(rect));
	BOOST_CHECK(null_rect.intersects_fast(rect));
	BOOST_CHECK(null_rect.intersects_fastest(rect));

	BOOST_CHECK(rect.intersects(null_rect));
	BOOST_CHECK(rect.intersects_fast(null_rect));
	BOOST_CHECK(rect.intersects_fastest(null_rect));

	null_rect = Rect(2, 2, 0, 0);

	BOOST_CHECK(!null_rect.intersects(rect));
	BOOST_CHECK(!null_rect.intersects_fast(rect));
	BOOST_CHECK(!null_rect.intersects_fastest(rect));

	BOOST_CHECK(!rect.intersects(null_rect));
	BOOST_CHECK(!rect.intersects_fast(null_rect));
	BOOST_CHECK(!rect.intersects_fastest(null_rect));
}
