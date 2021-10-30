// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/audio/format.h>
#include <yttrium/audio/utils.h>

#include <primal/buffer.hpp>

#include <array>
#include <cstring>

#include <doctest/doctest.h>

TEST_CASE("audio.transform.i16_to_i16x2")
{
	const std::array<int16_t, 17> srcData{
		0, 1, 2, 3, 4, 5, 6, 7, 8,
		9, 10, 11, 12, 13, 14, 15,
		16
	};

	const std::array<int16_t, 34> dstData{
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
		9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
		16, 16
	};

	primal::Buffer<int16_t> src{ srcData.size() };
	std::memcpy(src.data(), srcData.data(), srcData.size() * sizeof(int16_t));
	primal::Buffer<int16_t> dst{ dstData.size() };
	std::memset(dst.data(), 0xee, dstData.size() * sizeof(int16_t));
	REQUIRE(Yt::transform_audio(dst.data(), { Yt::AudioSample::i16, 2, 0 }, src.data(), { Yt::AudioSample::i16, 1, 0 }, srcData.size()));
	CHECK(std::memcmp(dst.data(), dstData.data(), dstData.size() * sizeof(int16_t)) == 0);
}

TEST_CASE("audio.transform.i16x2_to_i16x2")
{
	const std::array<uint16_t, 34> data{
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
		9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
		16, 16
	};

	primal::Buffer<int16_t> src{ data.size() };
	std::memcpy(src.data(), data.data(), data.size() * sizeof(int16_t));
	primal::Buffer<int16_t> dst{ data.size() };
	std::memset(dst.data(), 0xee, data.size() * sizeof(int16_t));
	REQUIRE(Yt::transform_audio(dst.data(), { Yt::AudioSample::i16, 2, 0 }, src.data(), { Yt::AudioSample::i16, 2, 0 }, data.size() / 2));
	CHECK(std::memcmp(dst.data(), data.data(), data.size() * sizeof(int16_t)) == 0);
}
