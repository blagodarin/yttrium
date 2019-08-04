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

#include <yttrium/test/utils.h>

#include <yttrium/memory/buffer.h>

#include <algorithm>
#include <cstring>
#include <random>

Yt::Buffer make_buffer(std::string_view text)
{
	return { text.size(), text.data() };
}

Yt::Buffer make_random_buffer(size_t size)
{
	std::default_random_engine engine;
	std::uniform_int_distribution<unsigned> distribution{ 0, std::numeric_limits<uint8_t>::max() }; // Visual C++ 2017 doesn't allow uint8_t distribution.
	Yt::Buffer buffer{ size };
	std::generate(buffer.begin(), buffer.end(), [&engine, &distribution] { return static_cast<uint8_t>(distribution(engine)); });
	return buffer;
}
