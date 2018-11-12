#include <yttrium/memory/buffer.h>
#include "../../libs/test/utils.h"

#include <cstring>

#include <catch2/catch.hpp>

TEST_CASE("test_utils.make_buffer")
{
	const std::string string = "test";
	const auto buffer = ::make_buffer(string);
	CHECK(buffer.size() == string.size());
	CHECK(!std::memcmp(buffer.data(), string.data(), string.size()));
}
