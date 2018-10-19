#include <yttrium/string_utils.h>

#include <cstring>
#include <limits>

#include <catch2/catch.hpp>

TEST_CASE("string_utils.make_string")
{
	using Yttrium::make_string;

	CHECK(make_string() == "");
	CHECK(make_string("") == "");
	CHECK(make_string(std::string{}) == "");

	CHECK(make_string("abc") == "abc");
	CHECK(make_string(std::string{"abc"}) == "abc");

	CHECK(make_string(short{ 0 }) == "0");
	CHECK(make_string(static_cast<unsigned short>(0)) == "0");
	CHECK(make_string(0) == "0");
	CHECK(make_string(0u) == "0");
	CHECK(make_string(0l) == "0");
	CHECK(make_string(0ul) == "0");
	CHECK(make_string(0ll) == "0");
	CHECK(make_string(0ull) == "0");

	CHECK(make_string('a') == "a");
	CHECK(make_string(static_cast<signed char>('a')) == "97");
	CHECK(make_string(static_cast<unsigned char>('a')) == "97");

	CHECK(make_string(std::numeric_limits<int8_t>::min()) == "-128");
	CHECK(make_string(std::numeric_limits<int8_t>::max()) == "127");
	CHECK(make_string(std::numeric_limits<uint8_t>::max()) == "255");

	CHECK(make_string(std::numeric_limits<int16_t>::min()) == "-32768");
	CHECK(make_string(std::numeric_limits<int16_t>::max()) == "32767");
	CHECK(make_string(std::numeric_limits<uint16_t>::max()) == "65535");

	CHECK(make_string(std::numeric_limits<int32_t>::min()) == "-2147483648");
	CHECK(make_string(std::numeric_limits<int32_t>::max()) == "2147483647");
	CHECK(make_string(std::numeric_limits<uint32_t>::max()) == "4294967295");

	CHECK(make_string(std::numeric_limits<int64_t>::min()) == "-9223372036854775808");
	CHECK(make_string(std::numeric_limits<int64_t>::max()) == "9223372036854775807");
	CHECK(make_string(std::numeric_limits<uint64_t>::max()) == "18446744073709551615");

	CHECK(make_string(0.0) == "0");
	CHECK(make_string(7.25f) == "7.25");
	CHECK(make_string(3.625) == "3.625");

	CHECK(make_string("abc", 123, "def", 4.5) == "abc123def4.5");
}

TEST_CASE("string_utils.ends_with")
{
	using namespace Yttrium::strings;

	CHECK(ends_with("", ""));
	CHECK(ends_with("test", ""));
	CHECK(ends_with("test", "t"));
	CHECK(!ends_with("test", "tes"));
	CHECK(ends_with("test", "test"));
	CHECK(!ends_with("test", "test "));
}

TEST_CASE("string_utils.to_int")
{
	using namespace Yttrium::strings;

	CHECK(to_int("") == 0);
	CHECK(to_int(std::to_string(std::numeric_limits<int>::min())) == std::numeric_limits<int>::min());
	CHECK(to_int(std::to_string(std::numeric_limits<int>::max())) == std::numeric_limits<int>::max());
}

TEST_CASE("string_utils.to_number.int32")
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

TEST_CASE("string_utils.to_number.uint32")
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

TEST_CASE("string_utils.to_number.double")
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

	CHECK(to_number("1.0", d));
	CHECK(d == 1.0);

	CHECK(to_number("3.2", d));
	CHECK(d == 3.2);

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

TEST_CASE("string_utils.to_time")
{
	// Helps sanitizer to catch out-of-bounds errors.
	const auto to_time = [](std::string_view text)
	{
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
