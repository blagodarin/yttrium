#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>
#include "iostream.h"

#include <boost/test/unit_test.hpp>

#include <array>

// TODO: Test Source properties.

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_package)
{
	Buffer buffer1(100003);
	::memset(buffer1.data(), 1, buffer1.size());

	Buffer buffer2(100019);
	::memset(buffer2.data(), 2, buffer2.size());

	Buffer buffer3(100043);
	::memset(buffer3.data(), 3, buffer3.size());

	TemporaryFile file1;
	Writer(file1).write_all(buffer1);

	TemporaryFile file2;
	Writer(file2).write_all(buffer2);

	TemporaryFile file3;
	Writer(file3).write_all(buffer3);

	TemporaryFile package_file;

	{
		const auto package_writer = PackageWriter::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package_writer);
		BOOST_REQUIRE(package_writer->add(file1.name(), {}));
		BOOST_REQUIRE(package_writer->add(file2.name(), {}));
		BOOST_REQUIRE(package_writer->add(file3.name(), {}));
		BOOST_REQUIRE(package_writer->commit());
	}

	std::unique_ptr<Source> packed_file1;
	std::unique_ptr<Source> packed_file2;
	std::unique_ptr<Source> packed_file3;

	{
		const auto package_reader = PackageReader::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package_reader);

		packed_file3 = package_reader->open(StaticString{ file3.name() });
		packed_file1 = package_reader->open(StaticString{ file1.name() });
		packed_file2 = package_reader->open(StaticString{ file2.name() });
	}

	BOOST_REQUIRE(packed_file1);
	BOOST_CHECK_EQUAL(packed_file1->to_buffer(), buffer1);

	packed_file1 = {};

	BOOST_REQUIRE(packed_file2);
	BOOST_CHECK_EQUAL(packed_file2->to_buffer(), buffer2);

	packed_file2 = {};

	BOOST_REQUIRE(packed_file3);
	BOOST_CHECK_EQUAL(packed_file3->to_buffer(), buffer3);
}

BOOST_AUTO_TEST_CASE(test_packed_file_size)
{
	TemporaryFile file1;
	Writer(file1).write_all(Buffer(1, "1"));

	TemporaryFile file2;
	Writer(file2).write_all(Buffer(1, "2"));

	TemporaryFile file3;
	Writer(file3).write_all(Buffer(1, "3"));

	TemporaryFile package_file;
	{
		const auto package_writer = PackageWriter::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package_writer);
		BOOST_REQUIRE(package_writer->add(file1.name(), {}));
		BOOST_REQUIRE(package_writer->add(file2.name(), {}));
		BOOST_REQUIRE(package_writer->add(file3.name(), {}));
		BOOST_REQUIRE(package_writer->commit());
	}

	const auto package_reader = PackageReader::create(package_file.name(), PackageType::Ypq);
	BOOST_REQUIRE(package_reader);

	auto packed_file = package_reader->open(StaticString{ file2.name() });
	BOOST_REQUIRE(packed_file);

	std::array<uint8_t, 2> data = { 0, 0 };
	BOOST_CHECK_EQUAL(packed_file->read_at(0, data.data(), 2), 1);
	BOOST_CHECK_EQUAL(data[0], '2');
}
