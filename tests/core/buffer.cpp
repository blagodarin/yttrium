#include <yttrium/memory/buffer.h>

#if Y_ENABLE_BUFFER_MEMORY_TRACKING
#	include "../../libs/core/memory/buffer_memory_tracker.h"
#endif

#include <catch2/catch.hpp>

using Yttrium::Buffer;

const auto granularity = Buffer::memory_granularity();

TEST_CASE("buffer")
{
	{
		Buffer buffer;
		CHECK(buffer.size() == 0);
		CHECK(buffer.capacity() == 0);
	}
	{
		Buffer buffer{ 0 };
		CHECK(buffer.size() == 0);
		CHECK(buffer.capacity() == 0);
	}
	{
		Buffer buffer{ 1 };
		CHECK(buffer.size() == 1);
		CHECK(buffer.capacity() == granularity);
	}
	{
		Buffer buffer{ granularity };
		CHECK(buffer.size() == granularity);
		CHECK(buffer.capacity() == granularity);
	}
	{
		Buffer buffer{ granularity + 1 };
		CHECK(buffer.size() == granularity + 1);
		CHECK(buffer.capacity() == granularity * 2);
	}
}

TEST_CASE("buffer.reserve")
{
	Buffer buffer;

	buffer.reserve(1);
	CHECK(buffer.size() == 0);
	CHECK(buffer.capacity() == granularity);

	const auto data1 = buffer.data();
	CHECK(data1);

	buffer.reserve(granularity);
	CHECK(buffer.size() == 0);
	CHECK(buffer.capacity() == granularity);
	CHECK(buffer.data() == data1);

	buffer.reserve(granularity + 1);
	CHECK(buffer.size() == 0);
	CHECK(buffer.capacity() == granularity * 2);

	const auto data2 = buffer.data();
	CHECK(data2);

	buffer.reserve(granularity);
	CHECK(buffer.size() == 0);
	CHECK(buffer.capacity() == granularity * 2);
	CHECK(buffer.data() == data2);
}

TEST_CASE("buffer.reset")
{
	Buffer buffer;

	buffer.reset(1);
	CHECK(buffer.size() == 1);
	CHECK(buffer.capacity() == granularity);

	buffer.reset(granularity + 1);
	CHECK(buffer.size() == granularity + 1);
	CHECK(buffer.capacity() == granularity * 2);
}

TEST_CASE("buffer.resize")
{
	Buffer buffer;

	buffer.resize(1);
	CHECK(buffer.size() == 1);
	CHECK(buffer.capacity() == granularity);

	buffer.resize(granularity + 1);
	CHECK(buffer.size() == granularity + 1);
	CHECK(buffer.capacity() == granularity * 2);

	const auto data_before_resize = buffer.data();
	buffer.resize(granularity);
	CHECK(buffer.size() == granularity);
	CHECK(buffer.capacity() == granularity * 2);
	CHECK(buffer.data() == data_before_resize);
}

TEST_CASE("buffer.shrink_to_fit")
{
	Buffer buffer(granularity * 2);
	CHECK(buffer.size() == granularity * 2);
	CHECK(buffer.capacity() == granularity * 2);

	buffer.resize(granularity);
	CHECK(buffer.size() == granularity);
	CHECK(buffer.capacity() == granularity * 2);

	buffer.shrink_to_fit();
	CHECK(buffer.size() == granularity);
	CHECK(buffer.capacity() == granularity);

	buffer.resize(0);
	CHECK(buffer.size() == 0);
	CHECK(buffer.capacity() == granularity);

	buffer.shrink_to_fit();
	CHECK(buffer.size() == 0);
	CHECK(buffer.capacity() == 0);
}

#if Y_ENABLE_BUFFER_MEMORY_TRACKING
TEST_CASE("buffer.total_capacity")
{
	CHECK(Yttrium::_buffer_memory_tracker.total_capacity() == 0);
	{
		Buffer buffer;
		CHECK(Yttrium::_buffer_memory_tracker.total_capacity() == 0);

		buffer.reserve(1);
		CHECK(Yttrium::_buffer_memory_tracker.total_capacity() == granularity);

		buffer.resize(1);
		CHECK(Yttrium::_buffer_memory_tracker.total_capacity() == granularity);

		buffer.resize(0);
		CHECK(Yttrium::_buffer_memory_tracker.total_capacity() == granularity);

		buffer.shrink_to_fit();
		CHECK(Yttrium::_buffer_memory_tracker.total_capacity() == 0);

		buffer = Buffer(granularity + 1);
		CHECK(Yttrium::_buffer_memory_tracker.total_capacity() == granularity * 2);
	}
	CHECK(Yttrium::_buffer_memory_tracker.total_capacity() == 0);
}
#endif
