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

BOOST_AUTO_TEST_CASE(marginsf_construction)
{
	using Yttrium::MarginsF;
	{
		MarginsF m;
		BOOST_CHECK_EQUAL(m._left, 0.f);
		BOOST_CHECK_EQUAL(m._top, 0.f);
		BOOST_CHECK_EQUAL(m._right, 0.f);
		BOOST_CHECK_EQUAL(m._bottom, 0.f);
	}
	{
		MarginsF m{1.f};
		BOOST_CHECK_EQUAL(m._left, 1.f);
		BOOST_CHECK_EQUAL(m._top, 1.f);
		BOOST_CHECK_EQUAL(m._right, 1.f);
		BOOST_CHECK_EQUAL(m._bottom, 1.f);
	}
	{
		MarginsF m{1.f, 2.f};
		BOOST_CHECK_EQUAL(m._left, 2.f);
		BOOST_CHECK_EQUAL(m._top, 1.f);
		BOOST_CHECK_EQUAL(m._right, 2.f);
		BOOST_CHECK_EQUAL(m._bottom, 1.f);
	}
	{
		MarginsF m{1.f, 2.f, 3.f};
		BOOST_CHECK_EQUAL(m._left, 2.f);
		BOOST_CHECK_EQUAL(m._top, 1.f);
		BOOST_CHECK_EQUAL(m._right, 2.f);
		BOOST_CHECK_EQUAL(m._bottom, 3.f);
	}
	{
		MarginsF m{1.f, 2.f, 3.f, 4.f};
		BOOST_CHECK_EQUAL(m._left, 4.f);
		BOOST_CHECK_EQUAL(m._top, 1.f);
		BOOST_CHECK_EQUAL(m._right, 2.f);
		BOOST_CHECK_EQUAL(m._bottom, 3.f);
	}
}
