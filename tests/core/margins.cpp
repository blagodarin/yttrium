#include <yttrium/math/margins.h>

#include <catch2/catch.hpp>

TEST_CASE("margins")
{
	using Yttrium::Margins;
	{
		Margins m;
		CHECK(m._left == 0);
		CHECK(m._top == 0);
		CHECK(m._right == 0);
		CHECK(m._bottom == 0);
	}
	{
		Margins m{1};
		CHECK(m._left == 1);
		CHECK(m._top == 1);
		CHECK(m._right == 1);
		CHECK(m._bottom == 1);
	}
	{
		Margins m{1, 2};
		CHECK(m._left == 2);
		CHECK(m._top == 1);
		CHECK(m._right == 2);
		CHECK(m._bottom == 1);
	}
	{
		Margins m{1, 2, 3};
		CHECK(m._left == 2);
		CHECK(m._top == 1);
		CHECK(m._right == 2);
		CHECK(m._bottom == 3);
	}
	{
		Margins m{1, 2, 3, 4};
		CHECK(m._left == 4);
		CHECK(m._top == 1);
		CHECK(m._right == 2);
		CHECK(m._bottom == 3);
	}
}

TEST_CASE("marginsf")
{
	using Yttrium::MarginsF;
	{
		MarginsF m;
		CHECK(m._left == 0.f);
		CHECK(m._top == 0.f);
		CHECK(m._right == 0.f);
		CHECK(m._bottom == 0.f);
	}
	{
		MarginsF m{1.f};
		CHECK(m._left == 1.f);
		CHECK(m._top == 1.f);
		CHECK(m._right == 1.f);
		CHECK(m._bottom == 1.f);
	}
	{
		MarginsF m{1.f, 2.f};
		CHECK(m._left == 2.f);
		CHECK(m._top == 1.f);
		CHECK(m._right == 2.f);
		CHECK(m._bottom == 1.f);
	}
	{
		MarginsF m{1.f, 2.f, 3.f};
		CHECK(m._left == 2.f);
		CHECK(m._top == 1.f);
		CHECK(m._right == 2.f);
		CHECK(m._bottom == 3.f);
	}
	{
		MarginsF m{1.f, 2.f, 3.f, 4.f};
		CHECK(m._left == 4.f);
		CHECK(m._top == 1.f);
		CHECK(m._right == 2.f);
		CHECK(m._bottom == 3.f);
	}
}
