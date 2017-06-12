#include <yttrium/memory/buffer_appender.h>
#include "../src/utils/fourcc.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(buffer_appender)
{
	using namespace Yttrium::Literals;

	Yttrium::Buffer b;
	Yttrium::BufferAppender<uint32_t> ba{b};
	BOOST_CHECK_EQUAL(0, ba.count());
	ba << "1234"_fourcc;
	ba << "5678"_fourcc;
	BOOST_CHECK_EQUAL(2, ba.count());
	BOOST_REQUIRE_EQUAL(8, b.size());
	BOOST_CHECK(!std::memcmp("12345678", b.data(), b.size()));
}
