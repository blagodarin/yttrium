//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/utils/string.h>
#include "../../src/utils/string.h"

#include <cstring>

#include <catch2/catch.hpp>

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

TEST_CASE("utils_string.to_number.uint32")
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
	CHECK(d == Approx{ 9.8e-1 }.epsilon(1e-13));

	CHECK(from_chars("98765.43210e-7", d));
	CHECK(d == Approx{ 98765.43210e-7 }.epsilon(1e-13));

	CHECK(!from_chars("+98765.43210e-7", d));

	CHECK(from_chars("-01234.56789e+7", d));
	CHECK(d == Approx{ -01234.56789e+7 }.epsilon(1e-13));

	CHECK(!from_chars("1 ", d));
	CHECK(!from_chars(" 1", d));

	CHECK(!from_chars("", d));
	CHECK(!from_chars("1e", d));
	CHECK(!from_chars("1eA", d));
}

TEST_CASE("utils_string.time_from_chars")
{
	// Helps sanitizer to catch out-of-bounds errors.
	const auto time_from_chars = [](std::string_view text) {
		const auto copy = std::make_unique<char[]>(text.size());
		std::memcpy(copy.get(), text.data(), text.size());
		return Yt::time_from_chars({ copy.get(), text.size() });
	};

	CHECK(time_from_chars("1") == 1'000);
	CHECK(time_from_chars("1:2") == 62'000);
	CHECK(time_from_chars("1:1:2") == 3662'000);
	CHECK(time_from_chars("1:1:2.5") == 3662'500);
	CHECK(time_from_chars("-10:1:2.25") == -36062'250);

	CHECK(time_from_chars("-01:23:45") == -5025'000);
	CHECK(time_from_chars("+01:23:45.") == 5025'000);
	CHECK(time_from_chars("-01:23:45.6") == -5025'600);
	CHECK(time_from_chars("+01:23:45.67") == 5025'670);
	CHECK(time_from_chars("-01:23:45.678") == -5025'678);
	CHECK(time_from_chars("+01:23:45.6789") == 5025'678);

	CHECK(time_from_chars("") == 0);
	CHECK(time_from_chars(".") == 0);
	CHECK(time_from_chars("+::.") == 0);
	CHECK(time_from_chars("+::.1") == 100);
	CHECK(time_from_chars("-1.1") == -1'100);
	CHECK(time_from_chars("-1:.1") == -60'100);
	CHECK(time_from_chars("-1::.1") == -3600'100);
	CHECK(time_from_chars("-1:::.1") == -3600'000);
}
