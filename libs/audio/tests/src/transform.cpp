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

#include <yttrium/audio/format.h>
#include <yttrium/audio/utils.h>
#include <yttrium/memory/buffer.h>

#include <array>
#include <cstring>

#include <doctest.h>

TEST_CASE("audio.transform.i16_to_i16x2")
{
	const std::array<int16_t, 17> src_data{
		0, 1, 2, 3, 4, 5, 6, 7, 8,
		9, 10, 11, 12, 13, 14, 15,
		16
	};

	const std::array<int16_t, 34> dst_data{
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
		9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
		16, 16
	};

	Yt::Buffer src{ src_data.size() * sizeof(int16_t), src_data.data() };
	Yt::Buffer dst{ dst_data.size() * sizeof(int16_t) };
	std::memset(dst.data(), 0xee, dst.size());
	REQUIRE(Yt::transform_audio(dst.data(), { Yt::AudioSample::i16, 2, 0 }, src.data(), { Yt::AudioSample::i16, 1, 0 }, src_data.size()));
	CHECK(std::memcmp(dst.data(), dst_data.data(), dst.size()) == 0);
}

TEST_CASE("audio.transform.i16x2_to_i16x2")
{
	const std::array<uint16_t, 34> data{
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
		9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
		16, 16
	};

	Yt::Buffer src{ data.size() * sizeof(int16_t), data.data() };
	Yt::Buffer dst{ data.size() * sizeof(int16_t) };
	std::memset(dst.data(), 0xee, dst.size());
	REQUIRE(Yt::transform_audio(dst.data(), { Yt::AudioSample::i16, 2, 0 }, src.data(), { Yt::AudioSample::i16, 2, 0 }, data.size()));
	CHECK(std::memcmp(dst.data(), data.data(), dst.size()) == 0);
}
