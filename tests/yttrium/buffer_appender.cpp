#include <yttrium/memory/buffer_appender.h>
#include "../../libs/yttrium/utils/fourcc.h"

#include <cstring>

#include <catch2/catch.hpp>

TEST_CASE("buffer_appender")
{
	using namespace Yttrium::Literals;

	Yttrium::Buffer b;
	Yttrium::BufferAppender<uint32_t> ba{b};
	CHECK(ba.count() == 0);
	ba << "1234"_fourcc;
	ba << "5678"_fourcc;
	CHECK(ba.count() == 2);
	REQUIRE(b.size() == 8);
	CHECK(!std::memcmp("12345678", b.data(), b.size()));
}
