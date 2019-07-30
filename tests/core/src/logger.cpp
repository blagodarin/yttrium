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

#include "../../../libs/core/src/logger/ring_log.h"

#include <catch2/catch.hpp>

TEST_CASE("logger.ring_log")
{
	constexpr size_t buffer_size = Yttrium::RingLog::BufferSize;
	constexpr size_t string_size = 251; // A prime number.
	static_assert(string_size <= Yttrium::RingLog::MaxStringSize);
	constexpr size_t max_strings = buffer_size / string_size;

	Yttrium::RingLog log;
	CHECK(log.empty());

	std::string string;
	CHECK(!log.pop(string));

	char next = 'A';
	for (size_t i = 0; i < max_strings + 1; ++i)
	{
		log.push(std::string(string_size, next));
		CHECK(!log.empty());
		next = next == 'Z' ? 'A' : static_cast<char>(next + 1);
	}

	next = 'B';
	for (size_t i = 0; i < max_strings; ++i)
	{
		CHECK(!log.empty());
		CHECK(log.pop(string));
		CHECK(string == std::string(string_size, next));
		next = next == 'Z' ? 'A' : static_cast<char>(next + 1);
	}

	CHECK(log.empty());
	CHECK(!log.pop(string));
}
