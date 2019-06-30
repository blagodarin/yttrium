//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/memory/buffer_appender.h>
#include "../../../libs/core/src/utils/fourcc.h"

#include <cstring>

#include <catch2/catch.hpp>

TEST_CASE("buffer_appender")
{
	using namespace Yttrium::Literals;

	Yttrium::Buffer b;
	Yttrium::BufferAppender<uint32_t> ba{ b };
	CHECK(ba.count() == 0);
	ba << "1234"_fourcc;
	ba << "5678"_fourcc;
	CHECK(ba.count() == 2);
	REQUIRE(b.size() == 8);
	CHECK(!std::memcmp("12345678", b.data(), b.size()));
}
