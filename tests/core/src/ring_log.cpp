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

#include "../../../libs/core/src/utils/ring_log.h"

#include <catch2/catch.hpp>

TEST_CASE("ring_log")
{
	Yttrium::RingLog log;
	size_t expected_offset = 0;
	size_t expected_size = 0;

	std::string string;
	CHECK(!log.pop(string));
	CHECK(log.offset() == 0);
	CHECK(log.size() == expected_size);

	log.push(std::string(254, 'A'));
	expected_size += 255;
	CHECK(log.offset() == expected_offset);
	CHECK(log.size() == expected_size);

	log.push(std::string(255, 'B'));
	expected_size += 256;
	CHECK(log.offset() == expected_offset);
	CHECK(log.size() == expected_size);

	while (expected_size < 4096 - 256)
	{
		log.push(std::string(256, 'C'));
		expected_size += 256;
		CHECK(log.offset() == expected_offset);
		CHECK(log.size() == expected_size);
	}

	CHECK(log.size() == 4095);

	log.push(std::string(255, 'D'));
	expected_offset += 255;
	expected_size = 4096;
	CHECK(log.offset() == 255);
	CHECK(log.size() == expected_size);

	log.push(std::string(254, 'E'));
	expected_offset += 256;
	expected_size = 4095;
	CHECK(log.offset() == expected_offset);
	CHECK(log.size() == expected_size);

	log.push(std::string(253, 'F'));
	expected_offset += 256;
	expected_size = 4093;
	CHECK(log.offset() == expected_offset);
	CHECK(log.size() == expected_size);

	while (expected_offset < 4095)
	{
		REQUIRE(log.pop(string));
		CHECK(string == std::string(255, 'C'));
		expected_offset += 256;
		expected_size -= 256;
		CHECK(log.offset() == expected_offset);
		CHECK(log.size() == expected_size);
	}

	REQUIRE(log.pop(string));
	CHECK(string == std::string(255, 'D'));
	expected_offset = 255;
	expected_size -= 256;
	CHECK(log.offset() == expected_offset);
	CHECK(log.size() == expected_size);

	REQUIRE(log.pop(string));
	CHECK(string == std::string(254, 'E'));
	expected_offset += 255;
	expected_size -= 255;
	CHECK(log.offset() == expected_offset);
	CHECK(log.size() == expected_size);

	REQUIRE(log.pop(string));
	CHECK(string == std::string(253, 'F'));
	expected_offset += 254;
	expected_size -= 254;
	CHECK(log.offset() == expected_offset);
	CHECK(log.size() == expected_size);
}
