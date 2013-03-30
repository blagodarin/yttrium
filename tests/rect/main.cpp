#include <Yttrium/rect.h>

#define BOOST_TEST_MODULE rect

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(rect_test)
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

	BOOST_CHECK(rect.contains(rect));

	Rect<int> another_rect(4, 5, 6, 1);

	BOOST_CHECK(rect.contains(another_rect));
	BOOST_CHECK(!another_rect.contains(rect));
}

BOOST_AUTO_TEST_CASE(intersection_test)
{
	Rect<int> rect(0, 0, 10, 7);

	BOOST_CHECK(rect.intersects(rect));
	BOOST_CHECK(rect.fast_intersects(rect));

	Rect<int> another_rect(7, 4, 10, 7);

	BOOST_CHECK(rect.intersects(another_rect));
	BOOST_CHECK(another_rect.intersects(rect));
	BOOST_CHECK(rect.fast_intersects(another_rect));
	BOOST_CHECK(another_rect.fast_intersects(rect));

	Rect<int> inner_rect(2, 2, 6, 3);

	BOOST_CHECK(rect.intersects(inner_rect));
	BOOST_CHECK(inner_rect.intersects(rect));
	BOOST_CHECK(rect.fast_intersects(inner_rect));
	BOOST_CHECK(inner_rect.fast_intersects(rect));

	Rect<int> outer_rect(-2, -2, 14, 11);

	BOOST_CHECK(rect.intersects(outer_rect));
	BOOST_CHECK(outer_rect.intersects(rect));
	BOOST_CHECK(rect.fast_intersects(outer_rect));
	BOOST_CHECK(outer_rect.fast_intersects(rect));

	Rect<int> right_rect(10, 0, 10, 7);

	BOOST_CHECK(!rect.intersects(right_rect));
	BOOST_CHECK(!right_rect.intersects(rect));
	BOOST_CHECK(!rect.fast_intersects(right_rect));
	BOOST_CHECK(!right_rect.fast_intersects(rect));

	Rect<int> bottom_rect(0, 7, 10, 7);

	BOOST_CHECK(!rect.intersects(bottom_rect));
	BOOST_CHECK(!bottom_rect.intersects(rect));
	BOOST_CHECK(!rect.fast_intersects(bottom_rect));
	BOOST_CHECK(!bottom_rect.fast_intersects(rect));

	Rect<int> far_rect(100, 100, 110, 107);

	BOOST_CHECK(!rect.intersects(far_rect));
	BOOST_CHECK(!far_rect.intersects(rect));
	BOOST_CHECK(!rect.fast_intersects(far_rect));
	BOOST_CHECK(!far_rect.fast_intersects(rect));

	Rect<int> null_rect;

	BOOST_CHECK(!null_rect.intersects(null_rect));
	BOOST_CHECK(null_rect.fast_intersects(null_rect)); // Different behavior.

	BOOST_CHECK(null_rect.intersects(outer_rect));
	BOOST_CHECK(outer_rect.intersects(null_rect));
	BOOST_CHECK(null_rect.fast_intersects(outer_rect));
	BOOST_CHECK(outer_rect.fast_intersects(null_rect));
}
