#include <yttrium/math/color.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(bgra32)
{
	using Yttrium::Bgra32;
	{
		Bgra32 color{1, 2, 3};
		BOOST_CHECK_EQUAL(color._b, 1u);
		BOOST_CHECK_EQUAL(color._g, 2u);
		BOOST_CHECK_EQUAL(color._r, 3u);
		BOOST_CHECK_EQUAL(color._a, 255u);
	}
	{
		Bgra32 color{1, 2, 3, 4};
		BOOST_CHECK_EQUAL(color._b, 1u);
		BOOST_CHECK_EQUAL(color._g, 2u);
		BOOST_CHECK_EQUAL(color._r, 3u);
		BOOST_CHECK_EQUAL(color._a, 4u);
	}
	BOOST_CHECK(Bgra32(1, 2, 3) == Bgra32(1, 2, 3, 0xff));
	BOOST_CHECK(Bgra32(1, 2, 3, 4) != Bgra32(1, 2, 3, 0xff));
}
