#include "../../libs/yttrium/utils/string.h"

#include <cstring>

#include <catch2/catch.hpp>

#include "../../libs/yttrium/utils/string.cpp"

TEST_CASE("utils.string.ends_with")
{
	using namespace Yttrium::strings;

	CHECK(ends_with("", ""));
	CHECK(ends_with("test", ""));
	CHECK(ends_with("test", "t"));
	CHECK(!ends_with("test", "tes"));
	CHECK(ends_with("test", "test"));
	CHECK(!ends_with("test", "test "));
}

TEST_CASE("utils.string.to_number.int32")
{
	using namespace Yttrium::strings;

	int32_t i = 0;

	CHECK(!to_number("-9999999999", i));
	CHECK(!to_number("-2147483649", i));

	CHECK(to_number("-2147483648", i));
	CHECK(i == -2147483647 - 1);

	CHECK(to_number("2147483647", i));
	CHECK(i == 2147483647);

	CHECK(!to_number("+2147483647", i));

	CHECK(!to_number("2147483648", i));
	CHECK(!to_number("+2147483648", i));
	CHECK(!to_number("9999999999", i));
	CHECK(!to_number("+9999999999", i));

	CHECK(!to_number("1 ", i));
	CHECK(!to_number(" 1", i));

	CHECK(!to_number("", i));
	CHECK(!to_number("+", i));
	CHECK(!to_number("-", i));
	CHECK(!to_number("A", i));
}

TEST_CASE("utils.string.to_number.uint32")
{
	using namespace Yttrium::strings;

	uint32_t u = 1;

	CHECK(!to_number("-0", u));

	CHECK(to_number("0", u));
	CHECK(u == 0u);

	CHECK(!to_number("+0", u));

	CHECK(to_number("4294967295", u));
	CHECK(u == 4294967295u);

	CHECK(!to_number("+4294967295", u));

	CHECK(!to_number("4294967296", u));
	CHECK(!to_number("+4294967296", u));
	CHECK(!to_number("9999999999", u));
	CHECK(!to_number("+9999999999", u));

	CHECK(!to_number("1 ", u));
	CHECK(!to_number(" 1", u));

	CHECK(!to_number("", u));
	CHECK(!to_number("+", u));
	CHECK(!to_number("A", u));
}

TEST_CASE("utils.string.to_number.double")
{
	using namespace Yttrium::strings;

	double d = -1.;

	CHECK(to_number("0", d));
	CHECK(d == 0.0);

	CHECK(to_number("0.0", d));
	CHECK(d == 0.0);

	CHECK(to_number("0.", d));
	CHECK(d == 0.0);

	CHECK(to_number(".0", d));
	CHECK(d == 0.0);

	CHECK(to_number("-.0", d));
	CHECK(d == -0.0);

	CHECK(to_number("1.0", d));
	CHECK(d == 1.0);

	CHECK(to_number("3.2", d));
	CHECK(d == 3.2);

	CHECK(!to_number(".", d));
	CHECK(!to_number(".e1", d));
	CHECK(!to_number("-", d));
	CHECK(!to_number("-.", d));
	CHECK(!to_number("-.e1", d));

	CHECK(to_number("5.4e1", d));
	CHECK(d == 5.4e1);

	CHECK(to_number("7.6e+1", d));
	CHECK(d == 7.6e+1);

	CHECK(to_number("9.8e-1", d));
	CHECK(d == Approx{9.8e-1}.epsilon(1e-13));

	CHECK(to_number("98765.43210e-7", d));
	CHECK(d == Approx{98765.43210e-7}.epsilon(1e-13));

	CHECK(!to_number("+98765.43210e-7", d));

	CHECK(to_number("-01234.56789e+7", d));
	CHECK(d == Approx{-01234.56789e+7}.epsilon(1e-13));

	CHECK(!to_number("1 ", d));
	CHECK(!to_number(" 1", d));

	CHECK(!to_number("", d));
	CHECK(!to_number("1e", d));
	CHECK(!to_number("1eA", d));
}

TEST_CASE("utils.string.to_time")
{
	// Helps sanitizer to catch out-of-bounds errors.
	const auto to_time = [](std::string_view text) {
		const auto copy = std::make_unique<char[]>(text.size());
		std::memcpy(copy.get(), text.data(), text.size());
		return Yttrium::strings::to_time({copy.get(), text.size()});
	};

	CHECK(to_time("1") == 1'000);
	CHECK(to_time("1:2") == 62'000);
	CHECK(to_time("1:1:2") == 3662'000);
	CHECK(to_time("1:1:2.5") == 3662'500);
	CHECK(to_time("-10:1:2.25") == -36062'250);

	CHECK(to_time("-01:23:45") == -5025'000);
	CHECK(to_time("+01:23:45.") == 5025'000);
	CHECK(to_time("-01:23:45.6") == -5025'600);
	CHECK(to_time("+01:23:45.67") == 5025'670);
	CHECK(to_time("-01:23:45.678") == -5025'678);
	CHECK(to_time("+01:23:45.6789") == 5025'678);

	CHECK(to_time("") == 0);
	CHECK(to_time(".") == 0);
	CHECK(to_time("+::.") == 0);
	CHECK(to_time("+::.1") == 100);
	CHECK(to_time("-1.1") == -1'100);
	CHECK(to_time("-1:.1") == -60'100);
	CHECK(to_time("-1::.1") == -3600'100);
	CHECK(to_time("-1:::.1") == -3600'000);
}
