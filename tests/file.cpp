#include <yttrium/io/file.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/string.h>

#include <cstdlib>
#include <cstring>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Yttrium
{
	Buffer make_random_buffer(size_t size)
	{
		Buffer buffer(size);
		for (auto& byte : buffer)
			byte = rand() % UINT8_MAX;
		return buffer;
	}

	inline std::ostream& operator<<(std::ostream& stream, const Buffer& buffer)
	{
		return stream << "Buffer(" << buffer.size() << ")";
	}
}

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_file_copy_all_from)
{
	const auto buffer = ::make_random_buffer(100003);

	File input(File::Temporary);
	input.write(buffer.data(), buffer.size());
	input.seek(0);

	File output(File::Temporary);
	output.copy_all_from(input);

	const auto actual = File::read_to_buffer(output.name());
	BOOST_CHECK_EQUAL(actual, buffer);
}

BOOST_AUTO_TEST_CASE(test_file_read_all)
{
	const auto expected_buffer = ::make_random_buffer(100003);

	File file(File::Temporary);
	file.write(expected_buffer.data(), expected_buffer.size());

	Buffer actual_buffer;

	BOOST_REQUIRE(File(file.name()).read_all(&actual_buffer));
	BOOST_CHECK_EQUAL(actual_buffer, expected_buffer);

	String actual_string;

	BOOST_REQUIRE(File(file.name()).read_all(&actual_string));
	BOOST_CHECK_EQUAL(actual_string.size(), expected_buffer.size());
	BOOST_CHECK(!::memcmp(actual_string.text(), expected_buffer.data(), expected_buffer.size()));
}

BOOST_AUTO_TEST_CASE(test_file_read_to_buffer)
{
	const auto expected = ::make_random_buffer(Buffer::memory_granularity());

	File file(File::Temporary);
	file.write(expected.data(), expected.size());

	const auto actual = File::read_to_buffer(file.name());
	BOOST_CHECK_EQUAL(actual, expected);
	BOOST_REQUIRE(actual.capacity() > actual.size());
	BOOST_CHECK_EQUAL(actual[actual.size()], '\0');
}

BOOST_AUTO_TEST_CASE(test_file_special)
{
	File file(File::StdErr);
	BOOST_REQUIRE(file);
	BOOST_CHECK(file.name().is_empty());

	const auto text = "Standard error output test.\n"_s;
	BOOST_CHECK(file.write(text.text(), text.size()));
}

BOOST_AUTO_TEST_CASE(test_memory_file_reading)
{
	const auto original_buffer = ::make_random_buffer(Buffer::memory_granularity());

	Buffer file_buffer(original_buffer.size());
	std::memcpy(file_buffer.data(), original_buffer.data(), original_buffer.size());

	File file(std::move(file_buffer));
	BOOST_REQUIRE(file);

	Buffer read_buffer;
	BOOST_REQUIRE(file.read_all(&read_buffer));
	BOOST_CHECK_EQUAL(read_buffer, original_buffer);
}

BOOST_AUTO_TEST_CASE(test_memory_file_writing)
{
	File file{ Buffer() };
	BOOST_REQUIRE(file);
	BOOST_CHECK_EQUAL(file.size(), 0);

	const auto original_buffer = ::make_random_buffer(Buffer::memory_granularity());

	BOOST_REQUIRE(file.write_all(original_buffer));
	BOOST_CHECK_EQUAL(file.size(), original_buffer.size());

	Buffer read_buffer;
	BOOST_REQUIRE(file.seek(0));
	BOOST_REQUIRE(file.read_all(&read_buffer));
	BOOST_CHECK_EQUAL(read_buffer, original_buffer);
}
