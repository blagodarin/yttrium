#include <Yttrium/rect.h>

#define BOOST_TEST_MODULE rect

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(rect_test)
{
	Rect<int> null_rect;

	BOOST_CHECK_EQUAL(null_rect.left(), 0);
	BOOST_CHECK_EQUAL(null_rect.top(), 0);
	BOOST_CHECK_EQUAL(null_rect.right(), -1);
	BOOST_CHECK_EQUAL(null_rect.bottom(), -1);

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
