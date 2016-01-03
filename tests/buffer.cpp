#include <yttrium/memory/buffer.h>

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_buffer_reset)
{
	// NOTE: Hack! If this test runs after any other test that uses memory buffers,
	// the following check will fail. Unfortunately, there seems to be no other way
	// to test the memory tracking functionality.
	BOOST_REQUIRE_EQUAL(Buffer::max_total_memory_allocated(), 0);

	const auto granularity = Buffer::memory_granularity();

	BOOST_CHECK_EQUAL(Buffer::total_memory_allocated(), 0);

	Buffer buffer;
	BOOST_CHECK(!buffer);
	BOOST_CHECK_EQUAL(buffer.size(), 0);
	BOOST_CHECK_EQUAL(buffer.capacity(), 0);

	BOOST_CHECK_EQUAL(Buffer::total_memory_allocated(), 0);
	BOOST_CHECK_EQUAL(Buffer::max_total_memory_allocated(), 0);

	buffer.reset(1);
	BOOST_CHECK(buffer);
	BOOST_CHECK_EQUAL(buffer.size(), 1);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

	BOOST_CHECK_EQUAL(Buffer::total_memory_allocated(), granularity);
	BOOST_CHECK_EQUAL(Buffer::max_total_memory_allocated(), granularity);

	buffer.reset(granularity);
	BOOST_CHECK(buffer);
	BOOST_CHECK_EQUAL(buffer.size(), granularity);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

	BOOST_CHECK_EQUAL(Buffer::total_memory_allocated(), granularity);
	BOOST_CHECK_EQUAL(Buffer::max_total_memory_allocated(), granularity);

	buffer.reset(granularity + 1);
	BOOST_CHECK(buffer);
	BOOST_CHECK_EQUAL(buffer.size(), granularity + 1);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);

	BOOST_CHECK_EQUAL(Buffer::total_memory_allocated(), granularity * 2);
	BOOST_CHECK_EQUAL(Buffer::max_total_memory_allocated(), granularity * 2);

	buffer.reset(granularity);
	BOOST_CHECK(buffer);
	BOOST_CHECK_EQUAL(buffer.size(), granularity);
	BOOST_CHECK(buffer.capacity() >= granularity);

	BOOST_CHECK_EQUAL(Buffer::max_total_memory_allocated(), granularity * 2);

	buffer.shrink_to_fit();
	BOOST_CHECK(buffer);
	BOOST_CHECK_EQUAL(buffer.size(), granularity);
	BOOST_CHECK(buffer.capacity() == granularity);

	BOOST_CHECK_EQUAL(Buffer::total_memory_allocated(), granularity);
	BOOST_CHECK_EQUAL(Buffer::max_total_memory_allocated(), granularity * 2);
}