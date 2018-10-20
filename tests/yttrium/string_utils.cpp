#include <yttrium/string_utils.h>

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
