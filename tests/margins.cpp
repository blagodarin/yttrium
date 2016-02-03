#include <yttrium/math/margins.h>
#include <yttrium/math/size.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_margins)
{
	Margins m0;
	BOOST_CHECK_EQUAL(m0.left(), 0);
	BOOST_CHECK_EQUAL(m0.top(), 0);
	BOOST_CHECK_EQUAL(m0.right(), 0);
	BOOST_CHECK_EQUAL(m0.bottom(), 0);

	Margins m1(1);
	BOOST_CHECK_EQUAL(m1.left(), 1);
	BOOST_CHECK_EQUAL(m1.top(), 1);
	BOOST_CHECK_EQUAL(m1.right(), 1);
	BOOST_CHECK_EQUAL(m1.bottom(), 1);

	Margins m2(1, 2);
	BOOST_CHECK_EQUAL(m2.left(), 2);
	BOOST_CHECK_EQUAL(m2.top(), 1);
	BOOST_CHECK_EQUAL(m2.right(), 2);
	BOOST_CHECK_EQUAL(m2.bottom(), 1);

	Margins m3(1, 2, 3);
	BOOST_CHECK_EQUAL(m3.left(), 2);
	BOOST_CHECK_EQUAL(m3.top(), 1);
	BOOST_CHECK_EQUAL(m3.right(), 2);
	BOOST_CHECK_EQUAL(m3.bottom(), 3);

	Margins m4(1, 2, 3, 4);
	BOOST_CHECK_EQUAL(m4.left(), 4);
	BOOST_CHECK_EQUAL(m4.top(), 1);
	BOOST_CHECK_EQUAL(m4.right(), 2);
	BOOST_CHECK_EQUAL(m4.bottom(), 3);

	BOOST_CHECK_EQUAL(m4.min_size().width(), m4.left() + 1 + m4.right());
	BOOST_CHECK_EQUAL(m4.min_size().height(), m4.top() + 1 + m4.bottom());
}

BOOST_AUTO_TEST_CASE(test_marginsf)
{
	MarginsF m0;
	BOOST_CHECK_EQUAL(m0.left(), 0);
	BOOST_CHECK_EQUAL(m0.top(), 0);
	BOOST_CHECK_EQUAL(m0.right(), 0);
	BOOST_CHECK_EQUAL(m0.bottom(), 0);

	MarginsF m1(1);
	BOOST_CHECK_EQUAL(m1.left(), 1);
	BOOST_CHECK_EQUAL(m1.top(), 1);
	BOOST_CHECK_EQUAL(m1.right(), 1);
	BOOST_CHECK_EQUAL(m1.bottom(), 1);

	MarginsF m2(1, 2);
	BOOST_CHECK_EQUAL(m2.left(), 2);
	BOOST_CHECK_EQUAL(m2.top(), 1);
	BOOST_CHECK_EQUAL(m2.right(), 2);
	BOOST_CHECK_EQUAL(m2.bottom(), 1);

	MarginsF m3(1, 2, 3);
	BOOST_CHECK_EQUAL(m3.left(), 2);
	BOOST_CHECK_EQUAL(m3.top(), 1);
	BOOST_CHECK_EQUAL(m3.right(), 2);
	BOOST_CHECK_EQUAL(m3.bottom(), 3);

	MarginsF m4(1, 2, 3, 4);
	BOOST_CHECK_EQUAL(m4.left(), 4);
	BOOST_CHECK_EQUAL(m4.top(), 1);
	BOOST_CHECK_EQUAL(m4.right(), 2);
	BOOST_CHECK_EQUAL(m4.bottom(), 3);
}
