#include <yttrium/math/margins.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(margins_construction)
{
	using Yttrium::Margins;
	{
		Margins m;
		BOOST_CHECK_EQUAL(m._left, 0);
		BOOST_CHECK_EQUAL(m._top, 0);
		BOOST_CHECK_EQUAL(m._right, 0);
		BOOST_CHECK_EQUAL(m._bottom, 0);
	}
	{
		Margins m{1};
		BOOST_CHECK_EQUAL(m._left, 1);
		BOOST_CHECK_EQUAL(m._top, 1);
		BOOST_CHECK_EQUAL(m._right, 1);
		BOOST_CHECK_EQUAL(m._bottom, 1);
	}
	{
		Margins m{1, 2};
		BOOST_CHECK_EQUAL(m._left, 2);
		BOOST_CHECK_EQUAL(m._top, 1);
		BOOST_CHECK_EQUAL(m._right, 2);
		BOOST_CHECK_EQUAL(m._bottom, 1);
	}
	{
		Margins m{1, 2, 3};
		BOOST_CHECK_EQUAL(m._left, 2);
		BOOST_CHECK_EQUAL(m._top, 1);
		BOOST_CHECK_EQUAL(m._right, 2);
		BOOST_CHECK_EQUAL(m._bottom, 3);
	}
	{
		Margins m{1, 2, 3, 4};
		BOOST_CHECK_EQUAL(m._left, 4);
		BOOST_CHECK_EQUAL(m._top, 1);
		BOOST_CHECK_EQUAL(m._right, 2);
		BOOST_CHECK_EQUAL(m._bottom, 3);
	}
}

BOOST_AUTO_TEST_CASE(margins_min_size)
{
	Yttrium::Margins m{1, 2, 3, 4};
	BOOST_CHECK_EQUAL(Yttrium::min_size(m)._width, m._left + 1 + m._right);
	BOOST_CHECK_EQUAL(Yttrium::min_size(m)._height, m._top + 1 + m._bottom);
}

BOOST_AUTO_TEST_CASE(marginsf_construction)
{
	using Yttrium::MarginsF;
	{
		MarginsF m;
		BOOST_CHECK_EQUAL(m._left, 0);
		BOOST_CHECK_EQUAL(m._top, 0);
		BOOST_CHECK_EQUAL(m._right, 0);
		BOOST_CHECK_EQUAL(m._bottom, 0);
	}
	{
		MarginsF m{1};
		BOOST_CHECK_EQUAL(m._left, 1);
		BOOST_CHECK_EQUAL(m._top, 1);
		BOOST_CHECK_EQUAL(m._right, 1);
		BOOST_CHECK_EQUAL(m._bottom, 1);
	}
	{
		MarginsF m{1, 2};
		BOOST_CHECK_EQUAL(m._left, 2);
		BOOST_CHECK_EQUAL(m._top, 1);
		BOOST_CHECK_EQUAL(m._right, 2);
		BOOST_CHECK_EQUAL(m._bottom, 1);
	}
	{
		MarginsF m{1, 2, 3};
		BOOST_CHECK_EQUAL(m._left, 2);
		BOOST_CHECK_EQUAL(m._top, 1);
		BOOST_CHECK_EQUAL(m._right, 2);
		BOOST_CHECK_EQUAL(m._bottom, 3);
	}
	{
		MarginsF m{1, 2, 3, 4};
		BOOST_CHECK_EQUAL(m._left, 4);
		BOOST_CHECK_EQUAL(m._top, 1);
		BOOST_CHECK_EQUAL(m._right, 2);
		BOOST_CHECK_EQUAL(m._bottom, 3);
	}
}
