#include <Yttrium/rect.h>

#define BOOST_TEST_MODULE rect

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(rect_test)
{
	Rect<int> null_rect;

	BOOST_CHECK_EQUAL(null_rect.x1(), 0);
	BOOST_CHECK_EQUAL(null_rect.y1(), 0);
	BOOST_CHECK_EQUAL(null_rect.x2(), 0);
	BOOST_CHECK_EQUAL(null_rect.y2(), 0);

	Rect<int> rect(3, 5, 7, 8);

	BOOST_CHECK_EQUAL(rect.x1(), 3);
	BOOST_CHECK_EQUAL(rect.y1(), 5);
	BOOST_CHECK_EQUAL(rect.x2(), 3 + 7);
	BOOST_CHECK_EQUAL(rect.y2(), 5 + 8);

	BOOST_CHECK_EQUAL(rect.x(), 3);
	BOOST_CHECK_EQUAL(rect.y(), 5);
	BOOST_CHECK_EQUAL(rect.width(), 7);
	BOOST_CHECK_EQUAL(rect.height(), 8);

	BOOST_CHECK(rect.contains(rect));

	Rect<int> another_rect(4, 5, 6, 1);

	BOOST_CHECK(rect.contains(another_rect));
	BOOST_CHECK(!another_rect.contains(rect));
}
