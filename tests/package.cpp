#include <yttrium/io/file.h>
#include <yttrium/io/package.h>
#include <yttrium/io/reader.h>
#include <yttrium/io/writer.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/static_string.h>

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
	::memset(buffer1.data(), 1, buffer1.size());

	Buffer buffer2(100019);
	::memset(buffer2.data(), 2, buffer2.size());

	Buffer buffer3(100043);
	::memset(buffer3.data(), 3, buffer3.size());

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
		const auto package_writer = PackageWriter::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package_writer);
		{
			auto packed_file1 = package_writer->open(file1.name());
			BOOST_REQUIRE(packed_file1);
			BOOST_REQUIRE_EQUAL(packed_file1.write_all(file1), file1.size());
		}
		{
			auto packed_file2 = package_writer->open(file2.name());
			BOOST_REQUIRE(packed_file2);
			BOOST_REQUIRE_EQUAL(packed_file2.write_all(file2), file2.size());
		}
		{
			auto packed_file3 = package_writer->open(file3.name());
			BOOST_REQUIRE(packed_file3);
			BOOST_REQUIRE_EQUAL(packed_file3.write_all(file3), file3.size());
		}
	}

	Reader packed_file1;
	Reader packed_file2;
	Reader packed_file3;

	{
		const auto package_reader = PackageReader::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package_reader);

		packed_file3 = package_reader->open(file3.name());
		packed_file1 = package_reader->open(file1.name());
		packed_file2 = package_reader->open(file2.name());
	}

	Buffer actual;

	BOOST_REQUIRE(packed_file1);
	BOOST_REQUIRE(packed_file1.read_all(actual));
	BOOST_CHECK_EQUAL(actual, buffer1);

	packed_file1 = {};

	BOOST_REQUIRE(packed_file2);
	BOOST_REQUIRE(packed_file2.read_all(actual));
	BOOST_CHECK_EQUAL(actual, buffer2);

	packed_file2 = {};

	BOOST_REQUIRE(packed_file3);
	BOOST_REQUIRE(packed_file3.read_all(actual));
	BOOST_CHECK_EQUAL(actual, buffer3);
}

BOOST_AUTO_TEST_CASE(test_packed_file_size)
{
	File package_file(File::Temporary);
	{
		const auto package_writer = PackageWriter::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package_writer);
		{
			auto packed_file = package_writer->open("1");
			BOOST_REQUIRE(packed_file);
			BOOST_CHECK(packed_file.write<uint8_t>(1));
		}
		{
			auto packed_file = package_writer->open("2");
			BOOST_REQUIRE(packed_file);
			BOOST_CHECK(packed_file.write<uint8_t>(2));
		}
	}

	const auto package_reader = PackageReader::create(package_file.name(), PackageType::Ypq);
	BOOST_REQUIRE(package_reader);

	auto packed_file = package_reader->open("1");
	BOOST_REQUIRE(packed_file);

	std::array<uint8_t, 2> data = { 0, 0 };
	BOOST_CHECK_EQUAL(packed_file.read(data.data(), 2), 1);
	BOOST_CHECK_EQUAL(data[0], 1);
}
