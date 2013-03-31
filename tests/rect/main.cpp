#include <Yttrium/rect.h>

#define BOOST_TEST_MODULE rect

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(initialization_test)
{
	Rect<int> null_rect;

	BOOST_CHECK_EQUAL(null_rect.left(), 0);
	BOOST_CHECK_EQUAL(null_rect.top(), 0);
	BOOST_CHECK_EQUAL(null_rect.right(), 0);
	BOOST_CHECK_EQUAL(null_rect.bottom(), 0);

	Rect<int> rect(3, 5, 7, 8);

	BOOST_CHECK_EQUAL(rect.left(), 3);
	BOOST_CHECK_EQUAL(rect.top(), 5);
	BOOST_CHECK_EQUAL(rect.width(), 7);
	BOOST_CHECK_EQUAL(rect.height(), 8);

	BOOST_CHECK_EQUAL(rect.left() + rect.width(), rect.right());
	BOOST_CHECK_EQUAL(rect.top() + rect.height(), rect.bottom());
}

BOOST_AUTO_TEST_CASE(containment_test)
{
	Rect<int> rect(0, 0, 10, 10);

	BOOST_CHECK(rect.contains(rect));

	BOOST_CHECK(rect.contains(Vector2<int>(0, 0)));
	BOOST_CHECK(rect.fast_contains(Vector2<int>(0, 0)));

	BOOST_CHECK(!rect.contains(Vector2<int>(-1, 0)));
	BOOST_CHECK(!rect.fast_contains(Vector2<int>(-1, 0)));

	BOOST_CHECK(!rect.contains(Vector2<int>(0, -1)));
	BOOST_CHECK(!rect.fast_contains(Vector2<int>(0, -1)));

	BOOST_CHECK(rect.contains(Vector2<int>(9, 9)));
	BOOST_CHECK(rect.fast_contains(Vector2<int>(9, 9)));

	BOOST_CHECK(!rect.contains(Vector2<int>(10, 9)));
	BOOST_CHECK(!rect.fast_contains(Vector2<int>(10, 9)));

	BOOST_CHECK(!rect.contains(Vector2<int>(9, 10)));
	BOOST_CHECK(!rect.fast_contains(Vector2<int>(9, 10)));

	Rect<int> smaller_rect(1, 1, 9, 9);

	BOOST_CHECK(rect.contains(smaller_rect));

	BOOST_CHECK(!smaller_rect.contains(rect));

	smaller_rect = Rect<int>(0, 0, 9, 9);

	BOOST_CHECK(rect.contains(smaller_rect));

	BOOST_CHECK(!smaller_rect.contains(rect));
}

BOOST_AUTO_TEST_CASE(intersection_test)
{
	Rect<int> rect(0, 0, 10, 7);

	BOOST_CHECK(rect.intersects(rect));
	BOOST_CHECK(rect.fast_intersects(rect));
	BOOST_CHECK(rect.fastest_intersects(rect));

	Rect<int> another_rect(7, 4, 10, 7);

	BOOST_CHECK(rect.intersects(another_rect));
	BOOST_CHECK(rect.fast_intersects(another_rect));
	BOOST_CHECK(rect.fastest_intersects(another_rect));

	BOOST_CHECK(another_rect.intersects(rect));
	BOOST_CHECK(another_rect.fast_intersects(rect));
	BOOST_CHECK(another_rect.fastest_intersects(rect));

	Rect<int> inner_rect(2, 2, 6, 3);

	BOOST_CHECK(rect.intersects(inner_rect));
	BOOST_CHECK(rect.fast_intersects(inner_rect));
	BOOST_CHECK(rect.fastest_intersects(inner_rect));

	BOOST_CHECK(inner_rect.intersects(rect));
	BOOST_CHECK(inner_rect.fast_intersects(rect));
	BOOST_CHECK(inner_rect.fastest_intersects(rect));

	Rect<int> outer_rect(-2, -2, 14, 11);

	BOOST_CHECK(rect.intersects(outer_rect));
	BOOST_CHECK(rect.fast_intersects(outer_rect));
	BOOST_CHECK(rect.fastest_intersects(outer_rect));

	BOOST_CHECK(outer_rect.intersects(rect));
	BOOST_CHECK(outer_rect.fast_intersects(rect));
	BOOST_CHECK(outer_rect.fastest_intersects(rect));

	Rect<int> right_border_rect(9, 0, 10, 7);

	BOOST_CHECK(rect.intersects(right_border_rect));
	BOOST_CHECK(rect.fast_intersects(right_border_rect));
	BOOST_CHECK(rect.fastest_intersects(right_border_rect));

	BOOST_CHECK(right_border_rect.intersects(rect));
	BOOST_CHECK(right_border_rect.fast_intersects(rect));
	BOOST_CHECK(right_border_rect.fastest_intersects(rect));

	Rect<int> right_rect(10, 0, 10, 7);

	BOOST_CHECK(!rect.intersects(right_rect));
	BOOST_CHECK(!rect.fast_intersects(right_rect));
	BOOST_CHECK(!rect.fastest_intersects(right_rect));

	BOOST_CHECK(!right_rect.intersects(rect));
	BOOST_CHECK(!right_rect.fast_intersects(rect));
	BOOST_CHECK(!right_rect.fastest_intersects(rect));

	Rect<int> bottom_border_rect(0, 6, 10, 7);

	BOOST_CHECK(rect.intersects(bottom_border_rect));
	BOOST_CHECK(rect.fast_intersects(bottom_border_rect));
	BOOST_CHECK(rect.fastest_intersects(bottom_border_rect));

	BOOST_CHECK(bottom_border_rect.intersects(rect));
	BOOST_CHECK(bottom_border_rect.fast_intersects(rect));
	BOOST_CHECK(bottom_border_rect.fastest_intersects(rect));

	Rect<int> bottom_rect(0, 7, 10, 7);

	BOOST_CHECK(!rect.intersects(bottom_rect));
	BOOST_CHECK(!rect.fast_intersects(bottom_rect));
	BOOST_CHECK(!rect.fastest_intersects(bottom_rect));

	BOOST_CHECK(!bottom_rect.intersects(rect));
	BOOST_CHECK(!bottom_rect.fast_intersects(rect));
	BOOST_CHECK(!bottom_rect.fastest_intersects(rect));

	Rect<int> far_rect(100, 100, 110, 107);

	BOOST_CHECK(!rect.intersects(far_rect));
	BOOST_CHECK(!rect.fast_intersects(far_rect));
	BOOST_CHECK(!rect.fastest_intersects(far_rect));

	BOOST_CHECK(!far_rect.intersects(rect));
	BOOST_CHECK(!far_rect.fast_intersects(rect));
	BOOST_CHECK(!far_rect.fastest_intersects(rect));
}

BOOST_AUTO_TEST_CASE(null_intersection_test)
{
	// This test case exists only as an implementation reference,
	// one should not rely on null rect intersection detection!

	Rect<int> null_rect;

	BOOST_CHECK(!null_rect.intersects(null_rect));
	BOOST_CHECK(!null_rect.fast_intersects(null_rect));
	BOOST_CHECK(null_rect.fastest_intersects(null_rect)); // Note the difference.

	// A null rect intersects with a non-null rect if the top left coordinate of the
	// null rect lies within the non-null rect (and not on its zero-width border).
	// Note that a point is equivalent to the rect with width and height set to one, not zero!

	Rect<int> rect(0, 0, 2, 2);

	BOOST_CHECK(!null_rect.intersects(rect));
	BOOST_CHECK(!null_rect.fast_intersects(rect));
	BOOST_CHECK(!null_rect.fastest_intersects(rect));

	BOOST_CHECK(!rect.intersects(null_rect));
	BOOST_CHECK(!rect.fast_intersects(null_rect));
	BOOST_CHECK(!rect.fastest_intersects(null_rect));

	null_rect = Rect<int>(1, 1, 0, 0);

	BOOST_CHECK(null_rect.intersects(rect));
	BOOST_CHECK(null_rect.fast_intersects(rect));
	BOOST_CHECK(null_rect.fastest_intersects(rect));

	BOOST_CHECK(rect.intersects(null_rect));
	BOOST_CHECK(rect.fast_intersects(null_rect));
	BOOST_CHECK(rect.fastest_intersects(null_rect));

	null_rect = Rect<int>(2, 2, 0, 0);

	BOOST_CHECK(!null_rect.intersects(rect));
	BOOST_CHECK(!null_rect.fast_intersects(rect));
	BOOST_CHECK(!null_rect.fastest_intersects(rect));

	BOOST_CHECK(!rect.intersects(null_rect));
	BOOST_CHECK(!rect.fast_intersects(null_rect));
	BOOST_CHECK(!rect.fastest_intersects(null_rect));
}
