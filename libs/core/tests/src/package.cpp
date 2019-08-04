//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>
#include "iostream.h"

#include <array>
#include <cstring>

#include <catch2/catch.hpp>

using namespace Yt;

namespace
{
	std::unique_ptr<Source> open_packed(const PackageReader& package, std::string_view name)
	{
		const auto& names = package.names();
		return package.open(static_cast<std::size_t>(std::find(names.begin(), names.end(), name) - names.begin()));
	}
}

TEST_CASE("package")
{
	Buffer buffer1{ 100003 };
	::memset(buffer1.data(), 1, buffer1.size());

	Buffer buffer2{ 100019 };
	::memset(buffer2.data(), 2, buffer2.size());

	Buffer buffer3{ 100043 };
	::memset(buffer3.data(), 3, buffer3.size());

	TemporaryFile file1;
	Writer{ file1 }.write_all(buffer1);

	TemporaryFile file2;
	Writer{ file2 }.write_all(buffer2);

	TemporaryFile file3;
	Writer{ file3 }.write_all(buffer3);

	TemporaryFile package_file;

	{
		const auto package_writer = PackageWriter::create(package_file.path(), PackageType::Ypq);
		REQUIRE(package_writer);
		REQUIRE(package_writer->add(file1.path().string()));
		REQUIRE(package_writer->add(file2.path().string()));
		REQUIRE(package_writer->add(file3.path().string()));
		REQUIRE(package_writer->commit());
	}

	std::unique_ptr<Source> packed_file1;
	std::unique_ptr<Source> packed_file2;
	std::unique_ptr<Source> packed_file3;

	{
		const auto package = PackageReader::create(package_file.path(), PackageType::Ypq);
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
	TemporaryFile file1;
	Writer{ file1 }.write_all(Buffer(1, "1"));

	TemporaryFile file2;
	Writer{ file2 }.write_all(Buffer(1, "2"));

	TemporaryFile file3;
	Writer{ file3 }.write_all(Buffer(1, "3"));

	TemporaryFile package_file;
	{
		const auto package_writer = PackageWriter::create(package_file.path(), PackageType::Ypq);
		REQUIRE(package_writer);
		REQUIRE(package_writer->add(file1.path().string()));
		REQUIRE(package_writer->add(file2.path().string()));
		REQUIRE(package_writer->add(file3.path().string()));
		REQUIRE(package_writer->commit());
	}

	const auto package = PackageReader::create(package_file.path(), PackageType::Ypq);
	REQUIRE(package);

	auto packed_file = ::open_packed(*package, file2.path().string());
	REQUIRE(packed_file);

	std::array<uint8_t, 2> data{ 0, 0 };
	CHECK(packed_file->read_at(0, data.data(), data.size()) == 1);
	CHECK(data[0] == '2');
}

TEST_CASE("package.duplicates")
{
	TemporaryFile package_file;
	TemporaryFile file;
	{
		const auto package_writer = PackageWriter::create(package_file.path(), PackageType::Ypq);
		REQUIRE(package_writer);
		Writer{ file }.write_all(Buffer{ 1, "1" });
		REQUIRE(package_writer->add(file.path().string()));
		Writer{ file }.write_all(Buffer{ 2, "23" });
		REQUIRE(package_writer->add(file.path().string()));
		REQUIRE(package_writer->commit());
	}

	const auto package = PackageReader::create(package_file.path(), PackageType::Ypq);
	REQUIRE(package);

	auto packed_file = ::open_packed(*package, file.path().string());
	REQUIRE(packed_file);

	std::array<uint8_t, 3> data{ 0, 0, 0 };
	REQUIRE(packed_file->read_at(0, data.data(), data.size()) == 2);
	CHECK(data[0] == '2');
	CHECK(data[1] == '3');
}
