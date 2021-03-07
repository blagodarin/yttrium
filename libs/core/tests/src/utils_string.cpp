// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/utils/string.h>
#include "../../src/utils/string.h"

#include <cstring>
#include <memory>

#include <doctest.h>

TEST_CASE("utils_string.ends_with")
{
	using Yt::ends_with;

	CHECK(ends_with("", ""));
	CHECK(ends_with("test", ""));
	CHECK(ends_with("test", "t"));
	CHECK(!ends_with("test", "tes"));
	CHECK(ends_with("test", "test"));
	CHECK(!ends_with("test", "test "));
}

TEST_CASE("utils_string.make_string")
{
	using Yt::make_string;

	CHECK(make_string() == "");
	CHECK(make_string("") == "");
	CHECK(make_string(std::string{}) == "");

	CHECK(make_string("abc") == "abc");
	CHECK(make_string(std::string{ "abc" }) == "abc");

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

TEST_CASE("utils_string.from_chars.int32")
{
	using Yt::from_chars;

	int32_t i = 0;

	CHECK(!from_chars("-9999999999", i));
	CHECK(!from_chars("-2147483649", i));

	CHECK(from_chars("-2147483648", i));
	CHECK(i == -2147483647 - 1);

	CHECK(from_chars("2147483647", i));
	CHECK(i == 2147483647);

	CHECK(!from_chars("+2147483647", i));

	CHECK(!from_chars("2147483648", i));
	CHECK(!from_chars("+2147483648", i));
	CHECK(!from_chars("9999999999", i));
	CHECK(!from_chars("+9999999999", i));

	CHECK(!from_chars("1 ", i));
	CHECK(!from_chars(" 1", i));

	CHECK(!from_chars("", i));
	CHECK(!from_chars("+", i));
	CHECK(!from_chars("-", i));
	CHECK(!from_chars("A", i));
}

TEST_CASE("utils_string.from_chars.uint32")
{
	using Yt::from_chars;

	uint32_t u = 1;

	CHECK(!from_chars("-0", u));

	CHECK(from_chars("0", u));
	CHECK(u == 0u);

	CHECK(!from_chars("+0", u));

	CHECK(from_chars("4294967295", u));
	CHECK(u == 4294967295u);

	CHECK(!from_chars("+4294967295", u));

	CHECK(!from_chars("4294967296", u));
	CHECK(!from_chars("+4294967296", u));
	CHECK(!from_chars("9999999999", u));
	CHECK(!from_chars("+9999999999", u));

	CHECK(!from_chars("1 ", u));
	CHECK(!from_chars(" 1", u));

	CHECK(!from_chars("", u));
	CHECK(!from_chars("+", u));
	CHECK(!from_chars("A", u));
}

TEST_CASE("utils_string.from_chars.double")
{
	using Yt::from_chars;

	double d = -1.;

	CHECK(from_chars("0", d));
	CHECK(d == 0.0);

	CHECK(from_chars("0.0", d));
	CHECK(d == 0.0);

	CHECK(from_chars("0.", d));
	CHECK(d == 0.0);

	CHECK(from_chars(".0", d));
	CHECK(d == 0.0);

	CHECK(from_chars("-.0", d));
	CHECK(d == -0.0);

	CHECK(from_chars("1.0", d));
	CHECK(d == 1.0);

	CHECK(from_chars("3.2", d));
	CHECK(d == 3.2);

	CHECK(!from_chars(".", d));
	CHECK(!from_chars(".e1", d));
	CHECK(!from_chars("-", d));
	CHECK(!from_chars("-.", d));
	CHECK(!from_chars("-.e1", d));

	CHECK(from_chars("5.4e1", d));
	CHECK(d == 5.4e1);

	CHECK(from_chars("7.6e+1", d));
	CHECK(d == 7.6e+1);

	CHECK(from_chars("9.8e-1", d));
	CHECK(d == doctest::Approx{ 9.8e-1 }.epsilon(1e-13));

	CHECK(from_chars("98765.43210e-7", d));
	CHECK(d == doctest::Approx{ 98765.43210e-7 }.epsilon(1e-13));

	CHECK(!from_chars("+98765.43210e-7", d));

	CHECK(from_chars("-01234.56789e+7", d));
	CHECK(d == doctest::Approx{ -01234.56789e+7 }.epsilon(1e-13));

	CHECK(!from_chars("1 ", d));
	CHECK(!from_chars(" 1", d));

	CHECK(!from_chars("", d));
	CHECK(!from_chars("1e", d));
	CHECK(!from_chars("1eA", d));
}
