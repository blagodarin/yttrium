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

#include <yttrium/memory/buffer.h>
#include "../../common/include/utils.h"

#include <cstring>

#include <catch2/catch.hpp>

TEST_CASE("test_utils.make_buffer")
{
	const std::string string = "test";
	const auto buffer = ::make_buffer(string);
	CHECK(buffer.size() == string.size());
	CHECK(!std::memcmp(buffer.data(), string.data(), string.size()));
}
