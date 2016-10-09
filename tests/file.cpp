#include <yttrium/io/file.h>
#include <yttrium/io/reader.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/string.h>

#include <cstdlib>
#include <cstring>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Yttrium
{
	inline Buffer make_random_buffer(size_t size)
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

	const auto actual = Reader(output.name()).to_buffer();
	BOOST_CHECK_EQUAL(actual, buffer);
}

BOOST_AUTO_TEST_CASE(test_file_special)
{
	File file(File::StdErr);
	BOOST_REQUIRE(file);
	BOOST_CHECK(file.name().is_empty());

	const auto text = "Standard error output test.\n"_s;
	BOOST_CHECK(file.write(text.text(), text.size()));
}
