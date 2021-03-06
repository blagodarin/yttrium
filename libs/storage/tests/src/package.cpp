// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/buffer.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary.h>
#include <yttrium/storage/writer.h>
#include <yttrium/storage/yp_writer.h>

#include <array>
#include <cstring>

#include <doctest.h>

// TODO: Test compressed packages.

namespace
{
	std::unique_ptr<Yt::Source> open_packed(const Yt::PackageReader& package, std::string_view name)
	{
		const auto& names = package.names();
		return package.open(static_cast<size_t>(std::find(names.begin(), names.end(), name) - names.begin()));
	}
}

TEST_CASE("package")
{
	Yt::Buffer buffer1{ 100003 };
	::memset(buffer1.data(), 1, buffer1.size());

	Yt::Buffer buffer2{ 100019 };
	::memset(buffer2.data(), 2, buffer2.size());

	Yt::Buffer buffer3{ 100043 };
	::memset(buffer3.data(), 3, buffer3.size());

	Yt::TemporaryFile file1;
	Yt::Writer{ file1 }.write_all(buffer1);

	Yt::TemporaryFile file2;
	Yt::Writer{ file2 }.write_all(buffer2);

	Yt::TemporaryFile file3;
	Yt::Writer{ file3 }.write_all(buffer3);

	Yt::TemporaryFile package_file;

	{
		Yt::YpWriter package_writer{ Yt::Writer{ package_file }, Yt::YpWriter::Compression::None };
		REQUIRE(package_writer.add(file1.path().string()));
		REQUIRE(package_writer.add(file2.path().string()));
		REQUIRE(package_writer.add(file3.path().string()));
		REQUIRE(package_writer.commit());
	}

	std::unique_ptr<Yt::Source> packed_file1;
	std::unique_ptr<Yt::Source> packed_file2;
	std::unique_ptr<Yt::Source> packed_file3;

	{
		const auto package = Yt::PackageReader::create(Yt::Source::from(package_file));
		REQUIRE(package);

		packed_file3 = ::open_packed(*package, file3.path().string());
		packed_file1 = ::open_packed(*package, file1.path().string());
		packed_file2 = ::open_packed(*package, file2.path().string());
	}

	REQUIRE(packed_file1);
	CHECK(packed_file1->to_buffer() == buffer1);

	packed_file1.reset();

	REQUIRE(packed_file2);
	CHECK(packed_file2->to_buffer() == buffer2);

	packed_file2.reset();

	REQUIRE(packed_file3);
	CHECK(packed_file3->to_buffer() == buffer3);
}

TEST_CASE("package.file_size")
{
	Yt::TemporaryFile file1;
	Yt::Writer{ file1 }.write_all(Yt::Buffer(1, "1"));

	Yt::TemporaryFile file2;
	Yt::Writer{ file2 }.write_all(Yt::Buffer(1, "2"));

	Yt::TemporaryFile file3;
	Yt::Writer{ file3 }.write_all(Yt::Buffer(1, "3"));

	Yt::TemporaryFile package_file;
	{
		Yt::YpWriter package_writer{ Yt::Writer{ package_file }, Yt::YpWriter::Compression::None };
		REQUIRE(package_writer.add(file1.path().string()));
		REQUIRE(package_writer.add(file2.path().string()));
		REQUIRE(package_writer.add(file3.path().string()));
		REQUIRE(package_writer.commit());
	}

	const auto package = Yt::PackageReader::create(Yt::Source::from(package_file));
	REQUIRE(package);

	auto packed_file = ::open_packed(*package, file2.path().string());
	REQUIRE(packed_file);

	std::array<uint8_t, 2> data{ 0, 0 };
	CHECK(packed_file->read_at(0, data.data(), data.size()) == 1);
	CHECK(data[0] == '2');
}

TEST_CASE("package.duplicates")
{
	Yt::TemporaryFile package_file;
	Yt::TemporaryFile file;
	{
		Yt::YpWriter package_writer{ Yt::Writer{ package_file }, Yt::YpWriter::Compression::None };
		Yt::Writer{ file }.write_all(Yt::Buffer{ 1, "1" });
		REQUIRE(package_writer.add(file.path().string()));
		Yt::Writer{ file }.write_all(Yt::Buffer{ 2, "23" });
		REQUIRE(package_writer.add(file.path().string()));
		REQUIRE(package_writer.commit());
	}

	const auto package = Yt::PackageReader::create(Yt::Source::from(package_file));
	REQUIRE(package);

	auto packed_file = ::open_packed(*package, file.path().string());
	REQUIRE(packed_file);

	std::array<uint8_t, 3> data{ 0, 0, 0 };
	REQUIRE(packed_file->read_at(0, data.data(), data.size()) == 2);
	CHECK(data[0] == '2');
	CHECK(data[1] == '3');
}
