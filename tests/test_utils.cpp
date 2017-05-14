#include "test_utils.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_make_buffer)
{
	const std::string string = "test";
	const auto buffer = ::make_buffer(string);
	BOOST_CHECK_EQUAL(buffer.size(), string.size());
	BOOST_CHECK(!std::memcmp(buffer.data(), string.data(), string.size()));
}
