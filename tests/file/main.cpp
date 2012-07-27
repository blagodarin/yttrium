#include <Yttrium/buffer.h>
#include <Yttrium/file.h>
#include <Yttrium/memory_manager.h>

#include <cstdlib> // rand
#include <cstring> // memcmp

#define BOOST_TEST_MODULE file

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_FIXTURE_TEST_SUITE(test_suite_memory_manager, MemoryManager)

BOOST_AUTO_TEST_CASE(file_transfer)
{
	uint8_t buffer[100003];

	for (size_t i = 0; i < Y_LENGTH_OF(buffer); ++i)
	{
		buffer[i] = rand() % UINT8_MAX;
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
