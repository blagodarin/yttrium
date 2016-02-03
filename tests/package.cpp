#include <yttrium/file.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/package.h>

#include <cstdlib>
#include <cstring>

#include <boost/test/unit_test.hpp>

namespace Yttrium
{
	inline std::ostream& operator<<(std::ostream& stream, const Buffer& buffer)
	{
		return stream << "Buffer(" << buffer.size() << ")";
	}
}

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_package)
{
	Buffer buffer1(100003);
	for (auto& byte : buffer1)
		byte = rand() % UINT8_MAX;

	Buffer buffer2(100019);
	for (auto& byte : buffer2)
		byte = rand() % UINT8_MAX;

	Buffer buffer3(100043);
	for (auto& byte : buffer3)
		byte = rand() % UINT8_MAX;

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

	{
		const auto& package_writer = PackageWriter::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package_writer);

		{
			File packed_file1 = package_writer->open_file(file1.name());
			BOOST_REQUIRE(packed_file1);
			FileTransfer<8192>(&packed_file1, &file1);
		}

		{
			File packed_file2 = package_writer->open_file(file2.name());
			BOOST_REQUIRE(packed_file2);
			FileTransfer<8192>(&packed_file2, &file2);
		}

		{
			File packed_file3 = package_writer->open_file(file3.name());
			BOOST_REQUIRE(packed_file3);
			FileTransfer<8192>(&packed_file3, &file3);
		}
	}

	File packed_file1;
	File packed_file2;
	File packed_file3;

	{
		const auto& package_reader = PackageReader::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package_reader);

		packed_file3 = package_reader->open_file(file3.name());
		packed_file1 = package_reader->open_file(file1.name());
		packed_file2 = package_reader->open_file(file2.name());
	}

	Buffer actual;

	BOOST_REQUIRE(packed_file1);
	BOOST_REQUIRE(packed_file1.read_all(&actual));
	BOOST_CHECK_EQUAL(actual, buffer1);

	packed_file1 = File();

	BOOST_REQUIRE(packed_file2);
	BOOST_REQUIRE(packed_file2.read_all(&actual));
	BOOST_CHECK_EQUAL(actual, buffer2);

	packed_file2 = File();

	BOOST_REQUIRE(packed_file3);
	BOOST_REQUIRE(packed_file3.read_all(&actual));
	BOOST_CHECK_EQUAL(actual, buffer3);

	packed_file3 = File();
}

// TODO: Test PackageManager.
