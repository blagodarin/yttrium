#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>
#include "iostream.h"

#include <boost/test/unit_test.hpp>

#include <array>

// TODO: Test Source properties.

using namespace Yttrium;

namespace
{
	std::unique_ptr<Source> open_packed(const PackageReader& package, const std::string& name)
	{
		const auto& names = package.names();
		return package.open(static_cast<std::size_t>(std::find(names.begin(), names.end(), StaticString{name}) - names.begin()));
	}
}

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
		const auto package = PackageReader::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package);

		packed_file3 = ::open_packed(*package, file3.name());
		packed_file1 = ::open_packed(*package, file1.name());
		packed_file2 = ::open_packed(*package, file2.name());
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

	const auto package = PackageReader::create(package_file.name(), PackageType::Ypq);
	BOOST_REQUIRE(package);

	auto packed_file = ::open_packed(*package, file2.name());
	BOOST_REQUIRE(packed_file);

	std::array<uint8_t, 2> data{0, 0};
	BOOST_CHECK_EQUAL(packed_file->read_at(0, data.data(), data.size()), 1);
	BOOST_CHECK_EQUAL(data[0], '2');
}

BOOST_AUTO_TEST_CASE(test_package_duplicates)
{
	TemporaryFile package_file;
	TemporaryFile file;
	{
		const auto package_writer = PackageWriter::create(package_file.name(), PackageType::Ypq);
		BOOST_REQUIRE(package_writer);
		Writer(file).write_all(Buffer(1, "1"));
		BOOST_REQUIRE(package_writer->add(file.name(), {}));
		Writer(file).write_all(Buffer(2, "23"));
		BOOST_REQUIRE(package_writer->add(file.name(), {}));
		BOOST_REQUIRE(package_writer->commit());
	}

	const auto package = PackageReader::create(package_file.name(), PackageType::Ypq);
	BOOST_REQUIRE(package);

	auto packed_file = ::open_packed(*package, file.name());
	BOOST_REQUIRE(packed_file);

	std::array<uint8_t, 3> data{0, 0, 0};
	BOOST_REQUIRE_EQUAL(packed_file->read_at(0, data.data(), data.size()), 2);
	BOOST_CHECK_EQUAL(data[0], '2');
	BOOST_CHECK_EQUAL(data[1], '3');
}
