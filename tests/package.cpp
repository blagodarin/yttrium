#include <yttrium/buffer.h>
#include <yttrium/file.h>
#include <yttrium/package.h>

#include <cstdlib> // rand
#include <cstring> // memcmp

#include "common.h"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_package_rewrite)
{
	DECLARE_MEMORY_MANAGER;

	std::array<uint8_t, 100003> buffer1;
	for (uint8_t& item: buffer1)
		item = rand() % UINT8_MAX;

	std::array<uint8_t, 100019> buffer2;
	for (uint8_t& item: buffer2)
		item = rand() % UINT8_MAX;

	std::array<uint8_t, 100043> buffer3;
	for (uint8_t& item: buffer3)
		item = rand() % UINT8_MAX;

	File file1(File::Temporary);
	file1.write(buffer1.data(), buffer1.size());
	file1.flush();
	file1.seek(0);

	File file2(File::Temporary);
	file2.write(buffer2.data(), buffer2.size());
	file2.flush();
	file2.seek(0);

	File file3(File::Temporary);
	file3.write(buffer3.data(), buffer3.size());
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
	BOOST_CHECK_EQUAL(actual.size(), buffer1.size());
	BOOST_CHECK(!::memcmp(actual.const_data(), buffer1.data(), buffer1.size()));

	packed_file1.close();

	BOOST_REQUIRE(packed_file2.is_opened());
	BOOST_REQUIRE(packed_file2.read_all(&actual));
	BOOST_CHECK_EQUAL(actual.size(), buffer2.size());
	BOOST_CHECK(!::memcmp(actual.const_data(), buffer2.data(), buffer2.size()));

	packed_file2.close();

	BOOST_REQUIRE(packed_file3.is_opened());
	BOOST_REQUIRE(packed_file3.read_all(&actual));
	BOOST_CHECK_EQUAL(actual.size(), buffer3.size());
	BOOST_CHECK(!::memcmp(actual.const_data(), buffer3.data(), buffer3.size()));

	packed_file3.close();
}
