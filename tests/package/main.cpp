#include <Yttrium/buffer.h>
#include <Yttrium/file.h>
#include <Yttrium/memory_manager.h>
#include <Yttrium/package.h>

#include <cstdlib> // rand
#include <cstring> // memcmp

#define BOOST_TEST_MODULE package

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_FIXTURE_TEST_SUITE(test_suite_memory_manager, MemoryManager)

BOOST_AUTO_TEST_CASE(rewrite_mode_test)
{
	uint8_t buffer1[100003];

	for (size_t i = 0; i < Y_LENGTH_OF(buffer1); ++i)
	{
		buffer1[i] = rand() % UINT8_MAX;
	}

	uint8_t buffer2[100019];

	for (size_t i = 0; i < Y_LENGTH_OF(buffer2); ++i)
	{
		buffer2[i] = rand() % UINT8_MAX;
	}

	uint8_t buffer3[100043];

	for (size_t i = 0; i < Y_LENGTH_OF(buffer3); ++i)
	{
		buffer3[i] = rand() % UINT8_MAX;
	}

	File file1(File::Temporary);

	file1.write(buffer1, sizeof(buffer1));
	file1.flush();
	file1.seek(0);

	File file2(File::Temporary);

	file2.write(buffer2, sizeof(buffer2));
	file2.flush();
	file2.seek(0);

	File file3(File::Temporary);

	file3.write(buffer3, sizeof(buffer3));
	file3.flush();
	file3.seek(0);

	File package_file(File::Temporary);

	PackageWriter package_writer;

	BOOST_REQUIRE(package_writer.open(package_file.name(), PackageType::Ypq, PackageWriter::Rewrite));

	File packed_file1 = package_writer.open_file(file1.name());

	BOOST_REQUIRE(packed_file1.is_opened());
	FileTransfer<8192>(&packed_file1, &file1);

	packed_file1.close();

	File packed_file2 = package_writer.open_file(file2.name());

	BOOST_REQUIRE(packed_file2.is_opened());
	FileTransfer<8192>(&packed_file2, &file2);

	packed_file2.close();

	File packed_file3 = package_writer.open_file(file3.name());

	BOOST_REQUIRE(packed_file3.is_opened());
	FileTransfer<8192>(&packed_file3, &file3);

	packed_file3.close();

	package_writer.close();

	PackageReader package_reader;

	BOOST_REQUIRE(package_reader.open(package_file.name(), PackageType::Ypq));

	Buffer actual;

	packed_file3 = package_reader.open_file(file3.name());
	packed_file1 = package_reader.open_file(file1.name());
	packed_file2 = package_reader.open_file(file2.name());

	package_reader.close();

	BOOST_REQUIRE(packed_file1.is_opened());
	BOOST_REQUIRE(packed_file1.read_all(&actual));
	BOOST_CHECK_EQUAL(actual.size(), Y_LENGTH_OF(buffer1));
	BOOST_CHECK(!memcmp(actual.const_data(), buffer1, Y_LENGTH_OF(buffer1)));

	packed_file1.close();

	BOOST_REQUIRE(packed_file2.is_opened());
	BOOST_REQUIRE(packed_file2.read_all(&actual));
	BOOST_CHECK_EQUAL(actual.size(), Y_LENGTH_OF(buffer2));
	BOOST_CHECK(!memcmp(actual.const_data(), buffer2, Y_LENGTH_OF(buffer2)));

	packed_file2.close();

	BOOST_REQUIRE(packed_file3.is_opened());
	BOOST_REQUIRE(packed_file3.read_all(&actual));
	BOOST_CHECK_EQUAL(actual.size(), Y_LENGTH_OF(buffer3));
	BOOST_CHECK(!memcmp(actual.const_data(), buffer3, Y_LENGTH_OF(buffer3)));

	packed_file3.close();
}

BOOST_AUTO_TEST_SUITE_END()