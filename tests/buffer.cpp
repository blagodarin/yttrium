#include <yttrium/memory/buffer.h>

#include "common.h"

using namespace Yttrium;

const auto granularity = Buffer::memory_granularity();

BOOST_AUTO_TEST_CASE(test_buffer_initialization)
{
	BOOST_REQUIRE_EQUAL(Buffer::total_capacity(), 0);
	{
		Buffer buffer;
		BOOST_CHECK_EQUAL(buffer.size(), 0);
		BOOST_CHECK_EQUAL(buffer.capacity(), 0);
		BOOST_CHECK_EQUAL(Buffer::total_capacity(), 0);
	}
	{
		Buffer buffer(0);
		BOOST_CHECK_EQUAL(buffer.size(), 0);
		BOOST_CHECK_EQUAL(buffer.capacity(), 0);
		BOOST_CHECK_EQUAL(Buffer::total_capacity(), 0);
	}
	{
		Buffer buffer(1);
		BOOST_CHECK_EQUAL(buffer.size(), 1);
		BOOST_CHECK_EQUAL(buffer.capacity(), granularity);
		BOOST_CHECK(Buffer::total_capacity() >= granularity);
	}
	BOOST_CHECK_EQUAL(Buffer::total_capacity(), 0);
	{
		Buffer buffer(granularity);
		BOOST_CHECK_EQUAL(buffer.size(), granularity);
		BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

		BOOST_CHECK(Buffer::total_capacity() >= granularity);
	}
	BOOST_CHECK_EQUAL(Buffer::total_capacity(), 0);
	{
		Buffer buffer(granularity + 1);
		BOOST_CHECK_EQUAL(buffer.size(), granularity + 1);
		BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);
		BOOST_CHECK(Buffer::total_capacity() >= granularity * 2);
	}
	BOOST_CHECK_EQUAL(Buffer::total_capacity(), 0);
}

BOOST_AUTO_TEST_CASE(test_buffer_reserve)
{
	Buffer buffer;

	buffer.reserve(1);
	BOOST_CHECK_EQUAL(buffer.size(), 0);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

	const auto data1 = buffer.data();
	BOOST_CHECK(data1);

	buffer.reserve(granularity);
	BOOST_CHECK_EQUAL(buffer.size(), 0);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);
	BOOST_CHECK_EQUAL(buffer.data(), data1);

	buffer.reserve(granularity + 1);
	BOOST_CHECK_EQUAL(buffer.size(), 0);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);

	const auto data2 = buffer.data();
	BOOST_CHECK(data2);

	buffer.reserve(granularity);
	BOOST_CHECK_EQUAL(buffer.size(), 0);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);
	BOOST_CHECK_EQUAL(buffer.data(), data2);
}

BOOST_AUTO_TEST_CASE(test_buffer_reset)
{
	Buffer buffer;

	buffer.reset(1);
	BOOST_CHECK_EQUAL(buffer.size(), 1);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

	buffer.reset(granularity + 1);
	BOOST_CHECK_EQUAL(buffer.size(), granularity + 1);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);
}

BOOST_AUTO_TEST_CASE(test_buffer_resize)
{
	Buffer buffer;

	buffer.resize(1);
	BOOST_CHECK_EQUAL(buffer.size(), 1);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

	buffer.resize(granularity + 1);
	BOOST_CHECK_EQUAL(buffer.size(), granularity + 1);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);

	const auto data_before_resize = buffer.data();
	buffer.resize(granularity);
	BOOST_CHECK_EQUAL(buffer.size(), granularity);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);
	BOOST_CHECK_EQUAL(buffer.data(), data_before_resize);
}

BOOST_AUTO_TEST_CASE(test_buffer_shrink_to_fit)
{
	BOOST_REQUIRE_EQUAL(Buffer::total_capacity(), 0);

	Buffer buffer(granularity * 2);
	BOOST_CHECK_EQUAL(buffer.size(), granularity * 2);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);

	buffer.resize(granularity);
	BOOST_CHECK_EQUAL(buffer.size(), granularity);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity * 2);

	buffer.shrink_to_fit();
	BOOST_CHECK_EQUAL(buffer.size(), granularity);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

	buffer.resize(0);
	BOOST_CHECK_EQUAL(buffer.size(), 0);
	BOOST_CHECK_EQUAL(buffer.capacity(), granularity);

	buffer.shrink_to_fit();
	BOOST_CHECK_EQUAL(buffer.size(), 0);
	BOOST_CHECK_EQUAL(buffer.capacity(), 0);

	BOOST_CHECK_EQUAL(Buffer::total_capacity(), 0);
}
