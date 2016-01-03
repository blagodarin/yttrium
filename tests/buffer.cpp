#include <yttrium/memory/buffer.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_buffer_reset)
{
	const auto granularity = Buffer::memory_granularity();

	Buffer buffer;
	BOOST_CHECK(!buffer);
	BOOST_CHECK_EQUAL(buffer.size(), 0);
	BOOST_CHECK_EQUAL(buffer.capacity(), 0);

	buffer.reset(1);
	BOOST_CHECK(buffer);
	BOOST_CHECK_EQUAL(buffer.size(), 1);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

	buffer.reset(granularity);
	BOOST_CHECK(buffer);
	BOOST_CHECK_EQUAL(buffer.size(), granularity);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

	buffer.reset(granularity + 1);
	BOOST_CHECK(buffer);
	BOOST_CHECK_EQUAL(buffer.size(), granularity + 1);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);

	buffer.reset(granularity);
	BOOST_CHECK(buffer);
	BOOST_CHECK_EQUAL(buffer.size(), granularity);
	BOOST_CHECK(buffer.capacity() >= granularity);
}
