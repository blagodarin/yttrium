#include <Yttrium/buffer.h>
#include <Yttrium/file.h>
#include <Yttrium/memory_manager.h>
#include <Yttrium/string.h>

#include <cstdlib> // rand
#include <cstring> // memcmp

#define BOOST_TEST_MODULE file

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_FIXTURE_TEST_SUITE(test_suite_memory_manager, MemoryManager)

BOOST_AUTO_TEST_CASE(read_all_test)
{
	uint8_t buffer[100003];

	for (uint8_t &item: buffer)
	{
		item = rand() % UINT8_MAX;
	}

	File file(File::Temporary);

	file.write(buffer, sizeof(buffer));
	file.flush();

	Buffer actual_buffer;

	BOOST_REQUIRE(File(file.name()).read_all(&actual_buffer));
	BOOST_CHECK_EQUAL(actual_buffer.size(), Y_LENGTH_OF(buffer));
	BOOST_CHECK(!memcmp(actual_buffer.const_data(), buffer, Y_LENGTH_OF(buffer)));

	String actual_string;

	BOOST_REQUIRE(File(file.name()).read_all(&actual_string));
	BOOST_CHECK_EQUAL(actual_string.size(), Y_LENGTH_OF(buffer));
	BOOST_CHECK(!memcmp(actual_string.const_text(), buffer, Y_LENGTH_OF(buffer)));
}

BOOST_AUTO_TEST_CASE(file_transfer_test)
{
	uint8_t buffer[100003];

	for (uint8_t &item: buffer)
	{
		item = rand() % UINT8_MAX;
	}

	File input(File::Temporary);

	input.write(buffer, sizeof(buffer));
	input.flush();
	input.seek(0);

	File output(File::Temporary);

	FileTransfer<8192>(&output, &input);

	Buffer actual;

	BOOST_REQUIRE(File(output.name()).read_all(&actual));
	BOOST_CHECK_EQUAL(actual.size(), Y_LENGTH_OF(buffer));
	BOOST_CHECK(!memcmp(actual.const_data(), buffer, Y_LENGTH_OF(buffer)));
}

BOOST_AUTO_TEST_SUITE_END()
