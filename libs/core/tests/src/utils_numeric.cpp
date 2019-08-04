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

#include <yttrium/utils/numeric.h>

#include <array>
#include <limits>

#include <catch2/catch.hpp>

TEST_CASE("utils.clamp_to_uint8")
{
	using Yttrium::clamp_to_uint8;

	CHECK(clamp_to_uint8(std::numeric_limits<int>::min()) == 0);
	CHECK(clamp_to_uint8(-1) == 0);
	CHECK(clamp_to_uint8(0) == 0);
	CHECK(clamp_to_uint8(1) == 1);
	CHECK(clamp_to_uint8(254) == 254);
	CHECK(clamp_to_uint8(255) == 255);
	CHECK(clamp_to_uint8(256) == 255);
	CHECK(clamp_to_uint8(std::numeric_limits<int>::max()) == 255);
}

TEST_CASE("utils.fixed_point")
{
	using Yttrium::fixed_point;

	CHECK(fixed_point<int, 4>(0.0000f) == 0b0000'0000);
	CHECK(fixed_point<int, 4>(0.0625f) == 0b0000'0001);
	CHECK(fixed_point<int, 4>(0.1250f) == 0b0000'0010);
	CHECK(fixed_point<int, 4>(0.2500f) == 0b0000'0100);
	CHECK(fixed_point<int, 4>(0.5000f) == 0b0000'1000);
	CHECK(fixed_point<int, 4>(1.0000f) == 0b0001'0000);
	CHECK(fixed_point<int, 4>(1.9375f) == 0b0001'1111);
}

TEST_CASE("utils.is_power_of_2")
{
	using Yttrium::is_power_of_2;

	static const std::array<int, std::numeric_limits<std::int8_t>::max() + 1> table{
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

	for (std::int8_t i = std::numeric_limits<std::int8_t>::min(); i < 0; ++i)
		CHECK(!is_power_of_2(i));
	for (std::uint8_t i = 0; i <= std::numeric_limits<std::int8_t>::max(); ++i)
		CHECK(int{ is_power_of_2(i) } == table[i]);
}

TEST_CASE("utils.make_cc")
{
	CHECK(Yttrium::make_cc('\x00', '\xff') == 0xff00);
	CHECK(Yttrium::make_cc('\x00', '\x80', '\x7f', '\xff') == 0xff7f8000);
	CHECK(Yttrium::make_cc('\x00', '\x01', '\x7e', '\x7f', '\x80', '\x81', '\xfe', '\xff') == 0xfffe81807f7e0100);
}

TEST_CASE("utils.next_power_of_2")
{
	using Yttrium::next_power_of_2;

	static const std::array<std::uint8_t, std::numeric_limits<std::int8_t>::max() + 1> table{
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

	for (std::size_t i = 1; i <= static_cast<std::size_t>(std::numeric_limits<std::int8_t>::max()); ++i)
		CHECK(next_power_of_2(i) == table[i]);

	const auto max_power_of_2 = static_cast<std::uintmax_t>(std::numeric_limits<std::intmax_t>::max()) + 1;
	CHECK(next_power_of_2(max_power_of_2 - 1) == max_power_of_2);
	CHECK(next_power_of_2(max_power_of_2) == max_power_of_2);
	CHECK(next_power_of_2(max_power_of_2 + 1) != max_power_of_2);
}

TEST_CASE("utils.power_of_2_alignment")
{
	using Yttrium::power_of_2_alignment;

	static const std::array<std::uint8_t, std::numeric_limits<std::int8_t>::max() + 1> table{
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

	for (int i = 0; i <= std::numeric_limits<std::int8_t>::max(); ++i)
	{
		CHECK(power_of_2_alignment(i) == table[static_cast<std::size_t>(i)]);
		CHECK(power_of_2_alignment(-i) == table[static_cast<std::size_t>(i)]);
	}
}

TEST_CASE("utils.same_sign")
{
	using Yttrium::same_sign;

	CHECK(same_sign<std::intmax_t>(0, 0));
	CHECK(same_sign<std::intmax_t>(-1, -1));
	CHECK(!same_sign<std::intmax_t>(0, -1));
	CHECK(!same_sign<std::intmax_t>(-1, 0));
	CHECK(same_sign(std::numeric_limits<std::intmax_t>::max(), std::numeric_limits<std::intmax_t>::max()));
	CHECK(same_sign(std::numeric_limits<std::intmax_t>::min(), std::numeric_limits<std::intmax_t>::min()));
	CHECK(!same_sign(std::numeric_limits<std::intmax_t>::max(), std::numeric_limits<std::intmax_t>::min()));
	CHECK(!same_sign(std::numeric_limits<std::intmax_t>::min(), std::numeric_limits<std::intmax_t>::max()));
}

TEST_CASE("utils.swap_bytes")
{
	using Yttrium::swap_bytes;

	CHECK(swap_bytes(std::uint16_t{ 0xff00 }) == 0x00ff);
	CHECK(swap_bytes(std::uint32_t{ 0xfff00f00 }) == 0x000ff0ff);
}

TEST_CASE("utils.wrap")
{
	using Yttrium::wrap;

	CHECK(wrap(-541, -180, 180) == 179);
	CHECK(wrap(-540, -180, 180) == -180);
	CHECK(wrap(-361, -180, 180) == -1);
	CHECK(wrap(-360, -180, 180) == 0);
	CHECK(wrap(-359, -180, 180) == 1);
	CHECK(wrap(-181, -180, 180) == 179);
	CHECK(wrap(-180, -180, 180) == -180);
	CHECK(wrap(-1, -180, 180) == -1);
	CHECK(wrap(0, -180, 180) == 0);
	CHECK(wrap(1, -180, 180) == 1);
	CHECK(wrap(179, -180, 180) == 179);
	CHECK(wrap(180, -180, 180) == -180);
	CHECK(wrap(359, -180, 180) == -1);
	CHECK(wrap(360, -180, 180) == 0);
	CHECK(wrap(361, -180, 180) == 1);
	CHECK(wrap(539, -180, 180) == 179);
	CHECK(wrap(540, -180, 180) == -180);
}