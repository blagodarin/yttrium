#include <yttrium/file.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/string.h>

#include <cstdlib>
#include <cstring>

#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_CASE(test_file_special)
{
	File file(File::StdErr);
	BOOST_REQUIRE(file);
	BOOST_CHECK(file.name().is_empty());

	const auto text = "Standard error output test.\n"_s;
	BOOST_CHECK(file.write(text.text(), text.size()));
}

BOOST_AUTO_TEST_CASE(test_file_read_all)
{
	const auto expected_buffer = make_random_buffer(100003);

	File file(File::Temporary);

	file.write(expected_buffer.data(), expected_buffer.size());
	file.flush();

	Buffer actual_buffer;

	BOOST_REQUIRE(File(file.name()).read_all(&actual_buffer));
	BOOST_CHECK_EQUAL(actual_buffer, expected_buffer);

	String actual_string;

	BOOST_REQUIRE(File(file.name()).read_all(&actual_string));
	BOOST_CHECK_EQUAL(actual_string.size(), expected_buffer.size());
	BOOST_CHECK(!::memcmp(actual_string.const_text(), expected_buffer.data(), expected_buffer.size()));
}

BOOST_AUTO_TEST_CASE(test_file_read_to_buffer)
{
	const auto expected = make_random_buffer(Buffer::memory_granularity());

	File file(File::Temporary);
	file.write(expected.data(), expected.size());
	file.flush();

	const auto actual = File::read_to_buffer(file.name());
	BOOST_CHECK_EQUAL(actual, expected);
	BOOST_REQUIRE(actual.capacity() > actual.size());
	BOOST_CHECK_EQUAL(actual[actual.size()], '\0');
}

BOOST_AUTO_TEST_CASE(test_file_transfer)
{
	const auto buffer = make_random_buffer(100003);

	File input(File::Temporary);

	input.write(buffer.data(), buffer.size());
	input.flush();
	input.seek(0);

	File output(File::Temporary);

	FileTransfer<8192>(&output, &input);

	const auto actual = File::read_to_buffer(output.name());
	BOOST_CHECK_EQUAL(actual, buffer);
}
