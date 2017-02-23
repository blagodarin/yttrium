#include <yttrium/math/matrix.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_matrix4)
{
	Matrix4 m{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

	BOOST_CHECK_EQUAL(m(0, 0), 0);
	BOOST_CHECK_EQUAL(m(0, 1), 1);
	BOOST_CHECK_EQUAL(m(0, 2), 2);
	BOOST_CHECK_EQUAL(m(0, 3), 3);
	BOOST_CHECK_EQUAL(m(1, 0), 4);
	BOOST_CHECK_EQUAL(m(1, 1), 5);
	BOOST_CHECK_EQUAL(m(1, 2), 6);
	BOOST_CHECK_EQUAL(m(1, 3), 7);
	BOOST_CHECK_EQUAL(m(2, 0), 8);
	BOOST_CHECK_EQUAL(m(2, 1), 9);
	BOOST_CHECK_EQUAL(m(2, 2), 10);
	BOOST_CHECK_EQUAL(m(2, 3), 11);
	BOOST_CHECK_EQUAL(m(3, 0), 12);
	BOOST_CHECK_EQUAL(m(3, 1), 13);
	BOOST_CHECK_EQUAL(m(3, 2), 14);
	BOOST_CHECK_EQUAL(m(3, 3), 15);

	BOOST_CHECK_EQUAL(m.data()[0], 0);
	BOOST_CHECK_EQUAL(m.data()[1], 4);
	BOOST_CHECK_EQUAL(m.data()[2], 8);
	BOOST_CHECK_EQUAL(m.data()[3], 12);
	BOOST_CHECK_EQUAL(m.data()[4], 1);
	BOOST_CHECK_EQUAL(m.data()[5], 5);
	BOOST_CHECK_EQUAL(m.data()[6], 9);
	BOOST_CHECK_EQUAL(m.data()[7], 13);
	BOOST_CHECK_EQUAL(m.data()[8], 2);
	BOOST_CHECK_EQUAL(m.data()[9], 6);
	BOOST_CHECK_EQUAL(m.data()[10], 10);
	BOOST_CHECK_EQUAL(m.data()[11], 14);
	BOOST_CHECK_EQUAL(m.data()[12], 3);
	BOOST_CHECK_EQUAL(m.data()[13], 7);
	BOOST_CHECK_EQUAL(m.data()[14], 11);
	BOOST_CHECK_EQUAL(m.data()[15], 15);
}
