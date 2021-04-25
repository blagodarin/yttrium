// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/numeric.h>

#include <array>
#include <limits>

#include <doctest.h>

TEST_CASE("utils.clamp_to_uint8")
{
	using Yt::clamp_to_uint8;

	CHECK(clamp_to_uint8(std::numeric_limits<int>::min()) == 0);
	CHECK(clamp_to_uint8(-1) == 0);
	CHECK(clamp_to_uint8(0) == 0);
	CHECK(clamp_to_uint8(1) == 1);
	CHECK(clamp_to_uint8(254) == 254);
	CHECK(clamp_to_uint8(255) == 255);
	CHECK(clamp_to_uint8(256) == 255);
	CHECK(clamp_to_uint8(std::numeric_limits<int>::max()) == 255);
}

TEST_CASE("utils.is_power_of_2")
{
	using Yt::is_power_of_2;

	static const std::array<int, std::numeric_limits<int8_t>::max() + 1> table{
		// clang-format off
		0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// clang-format on
	};

	for (int8_t i = std::numeric_limits<int8_t>::min(); i < 0; ++i)
		CHECK(!is_power_of_2(i));
	for (uint8_t i = 0; i <= std::numeric_limits<int8_t>::max(); ++i)
		CHECK(int{ is_power_of_2(i) } == table[i]);
}

TEST_CASE("utils.next_power_of_2")
{
	using Yt::next_power_of_2;

	static const std::array<uint8_t, std::numeric_limits<int8_t>::max() + 1> table{
		// clang-format off
		0, 1, 2, 4, 4, 8, 8, 8, 8, 16, 16, 16, 16, 16, 16, 16,
		16, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
		32, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
		128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
		// clang-format on
	};

	for (std::size_t i = 1; i <= static_cast<size_t>(std::numeric_limits<int8_t>::max()); ++i)
		CHECK(next_power_of_2(i) == table[i]);

	const auto max_power_of_2 = static_cast<uintmax_t>(std::numeric_limits<intmax_t>::max()) + 1;
	CHECK(next_power_of_2(max_power_of_2 - 1) == max_power_of_2);
	CHECK(next_power_of_2(max_power_of_2) == max_power_of_2);
	CHECK(next_power_of_2(max_power_of_2 + 1) != max_power_of_2);
}

TEST_CASE("utils.power_of_2_alignment")
{
	using Yt::power_of_2_alignment;

	static const std::array<uint8_t, std::numeric_limits<int8_t>::max() + 1> table{
		// clang-format off
		0, 1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1,
		16, 1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1,
		32, 1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1,
		16, 1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1,
		64, 1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1,
		16, 1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1,
		32, 1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1,
		16, 1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1,
		// clang-format on
	};

	for (int i = 0; i <= std::numeric_limits<int8_t>::max(); ++i)
	{
		CHECK(power_of_2_alignment(i) == table[static_cast<size_t>(i)]);
		CHECK(power_of_2_alignment(-i) == table[static_cast<size_t>(i)]);
	}
}

TEST_CASE("utils.same_sign")
{
	using Yt::same_sign;

	CHECK(same_sign<intmax_t>(0, 0));
	CHECK(same_sign<intmax_t>(-1, -1));
	CHECK(!same_sign<intmax_t>(0, -1));
	CHECK(!same_sign<intmax_t>(-1, 0));
	CHECK(same_sign(std::numeric_limits<intmax_t>::max(), std::numeric_limits<intmax_t>::max()));
	CHECK(same_sign(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::min()));
	CHECK(!same_sign(std::numeric_limits<intmax_t>::max(), std::numeric_limits<intmax_t>::min()));
	CHECK(!same_sign(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max()));
}
