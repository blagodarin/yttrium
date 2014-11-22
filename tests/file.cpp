#include <yttrium/buffer.h>
#include <yttrium/file.h>
#include <yttrium/memory_manager.h>
#include <yttrium/string.h>

#include <cstdlib> // rand
#include <cstring> // memcmp

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_file_special)
{
	DECLARE_MEMORY_MANAGER;

	File file(File::StdErr);
	BOOST_REQUIRE(file);

	const S text("Standard error output test.\n");
	BOOST_CHECK(file.write(text.text(), text.size()));
}

BOOST_AUTO_TEST_CASE(test_file_read_all)
{
	DECLARE_MEMORY_MANAGER;

	std::array<uint8_t, 100003> buffer;
	for (uint8_t& item: buffer)
		item = rand() % UINT8_MAX;

	File file(File::Temporary);

	file.write(buffer.data(), buffer.size());
	file.flush();

	Buffer actual_buffer;

	BOOST_REQUIRE(File(file.name()).read_all(&actual_buffer));
	BOOST_CHECK_EQUAL(actual_buffer.size(), buffer.size());
	BOOST_CHECK(!::memcmp(actual_buffer.const_data(), buffer.data(), buffer.size()));

	String actual_string;

	BOOST_REQUIRE(File(file.name()).read_all(&actual_string));
	BOOST_CHECK_EQUAL(actual_string.size(), buffer.size());
	BOOST_CHECK(!::memcmp(actual_string.const_text(), buffer.data(), buffer.size()));
}

BOOST_AUTO_TEST_CASE(test_file_transfer)
{
	DECLARE_MEMORY_MANAGER;

	std::array<uint8_t, 100003> buffer;
	for (uint8_t& item: buffer)
		item = rand() % UINT8_MAX;

	File input(File::Temporary);

	input.write(buffer.data(), buffer.size());
	input.flush();
	input.seek(0);

	File output(File::Temporary);

	FileTransfer<8192>(&output, &input);

	Buffer actual;

	BOOST_REQUIRE(File(output.name()).read_all(&actual));
	BOOST_CHECK_EQUAL(actual.size(), buffer.size());
	BOOST_CHECK(!::memcmp(actual.const_data(), buffer.data(), buffer.size()));
}
