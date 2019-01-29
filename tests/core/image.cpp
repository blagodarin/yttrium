//
// Copyright 2019 Sergei Blagodarin
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

#include "image.h"

#include <array>

#include <catch2/catch.hpp>

using Yttrium::Image;
using Yttrium::PixelFormat;

TEST_CASE("image.compare")
{
	{
		const std::uint8_t data = 0x01;
		const Image image1{ { 1, 1, PixelFormat::Gray8 }, &data };
		const Image image2{ { 1, 1, PixelFormat::Gray8 }, &data };
		CHECK(image1 == image2);
	}
	{
		const std::uint8_t data1 = 0x01;
		const std::uint8_t data2 = 0x02;
		const Image image1{ { 1, 1, PixelFormat::Gray8 }, &data1 };
		const Image image2{ { 1, 1, PixelFormat::Gray8 }, &data2 };
		CHECK(image1 != image2);
	}
	{
		const std::array<std::uint8_t, 2> data = { 0x01, 0x02 };
		const Image image1{ { 2, 1, PixelFormat::Gray8 }, data.data() };
		const Image image2{ { 1, 2, PixelFormat::Gray8 }, data.data() };
		CHECK(image1 != image2);
	}
}

TEST_CASE("image.intensity_to_bgra")
{
	const std::array<std::uint8_t, 8> intensity{
		// clang-format off
		0x00, 0x01, 0x02, 0x03,
		0x10, 0x11, 0x12, 0x13,
		// clang-format on
	};

	const std::array<std::uint8_t, 24> bgra{
		// clang-format off
		0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02,
		0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12,
		// clang-format on
	};

	const Image intensity_image{ { 3, 2, PixelFormat::Gray8, 4 }, intensity.data() };
	const Image bgra_image{ { 3, 2, PixelFormat::Bgra32 }, bgra.data() };

	CHECK(*intensity_to_bgra(intensity_image) == bgra_image);
	CHECK(!intensity_to_bgra(bgra_image));
}

TEST_CASE("image.to_bgra.bgr")
{
	const std::array<std::uint8_t, 24> bgr{
		// clang-format off
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
		// clang-format on
	};

	const std::array<std::uint8_t, 24> bgra{
		// clang-format off
		0x00, 0x01, 0x02, 0xff, 0x03, 0x04, 0x05, 0xff, 0x06, 0x07, 0x08, 0xff,
		0x10, 0x11, 0x12, 0xff, 0x13, 0x14, 0x15, 0xff, 0x16, 0x17, 0x18, 0xff,
		// clang-format on
	};

	const Image bgr_image{ { 3, 2, PixelFormat::Bgr24, 4 }, bgr.data() };
	const Image bgra_image{ { 3, 2, PixelFormat::Bgra32 }, bgra.data() };

	CHECK(to_bgra(bgr_image) == bgra_image);
}

TEST_CASE("image.to_bgra.bgra")
{
	const std::array<std::uint8_t, 24> bgra{
		// clang-format off
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
		// clang-format on
	};

	const Image bgra_image{ { 3, 2, PixelFormat::Bgra32 }, bgra.data() };

	CHECK(to_bgra(bgra_image) == bgra_image);
}

TEST_CASE("image.to_bgra.gray")
{
	const std::array<std::uint8_t, 8> gray{
		// clang-format off
		0x00, 0x01, 0x02, 0x03,
		0x10, 0x11, 0x12, 0x13,
		// clang-format on
	};

	const std::array<std::uint8_t, 24> bgra{
		// clang-format off
		0x00, 0x00, 0x00, 0xff, 0x01, 0x01, 0x01, 0xff, 0x02, 0x02, 0x02, 0xff,
		0x10, 0x10, 0x10, 0xff, 0x11, 0x11, 0x11, 0xff, 0x12, 0x12, 0x12, 0xff,
		// clang-format on
	};

	const Image gray_image{ { 3, 2, PixelFormat::Gray8, 4 }, gray.data() };
	const Image bgra_image{ { 3, 2, PixelFormat::Bgra32 }, bgra.data() };

	CHECK(to_bgra(gray_image) == bgra_image);
}

TEST_CASE("image.to_bgra.gray_alpha")
{
	const std::array<std::uint8_t, 16> gray_alpha{
		// clang-format off
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		// clang-format on
	};

	const std::array<std::uint8_t, 24> bgra{
		// clang-format off
		0x00, 0x00, 0x00, 0x01, 0x02, 0x02, 0x02, 0x03, 0x04, 0x04, 0x04, 0x05,
		0x10, 0x10, 0x10, 0x11, 0x12, 0x12, 0x12, 0x13, 0x14, 0x14, 0x14, 0x15,
		// clang-format on
	};

	const Image gray_alpha_image{ { 3, 2, PixelFormat::GrayAlpha16, 4 }, gray_alpha.data() };
	const Image bgra_image{ { 3, 2, PixelFormat::Bgra32 }, bgra.data() };

	CHECK(to_bgra(gray_alpha_image) == bgra_image);
}

TEST_CASE("image.to_bgra.rgb")
{
	const std::array<std::uint8_t, 24> rgb{
		// clang-format off
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
		// clang-format on
	};

	const std::array<std::uint8_t, 24> bgra{
		// clang-format off
		0x02, 0x01, 0x00, 0xff, 0x05, 0x04, 0x03, 0xff, 0x08, 0x07, 0x06, 0xff,
		0x12, 0x11, 0x10, 0xff, 0x15, 0x14, 0x13, 0xff, 0x18, 0x17, 0x16, 0xff,
		// clang-format on
	};

	const Image rgb_image{ { 3, 2, PixelFormat::Rgb24, 4 }, rgb.data() };
	const Image bgra_image{ { 3, 2, PixelFormat::Bgra32 }, bgra.data() };

	CHECK(to_bgra(rgb_image) == bgra_image);
}

TEST_CASE("image.to_bgra.rgba")
{
	const std::array<std::uint8_t, 24> rgba{
		// clang-format off
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
		// clang-format on
	};

	const std::array<uint8_t, 24> bgra{
		// clang-format off
		0x02, 0x01, 0x00, 0x03, 0x06, 0x05, 0x04, 0x07, 0x0a, 0x09, 0x08, 0x0b,
		0x12, 0x11, 0x10, 0x13, 0x16, 0x15, 0x14, 0x17, 0x1a, 0x19, 0x18, 0x1b,
		// clang-format on
	};

	const Image rgba_image{ { 3, 2, PixelFormat::Rgba32 }, rgba.data() };
	const Image bgra_image{ { 3, 2, PixelFormat::Bgra32 }, bgra.data() };

	CHECK(to_bgra(rgba_image) == bgra_image);
}

TEST_CASE("image.swap_channels.gray")
{
	const std::array<std::uint8_t, 8> gray{
		// clang-format off
		0x00, 0x01, 0x02, 0x03,
		0x10, 0x11, 0x12, 0x13,
		// clang-format on
	};

	Image gray_image({ 3, 2, PixelFormat::Gray8, 4 }, gray.data());
	REQUIRE(gray_image.swap_channels());
	CHECK(!std::memcmp(gray_image.data(), gray.data(), gray.size()));
}

TEST_CASE("image.swap_channels.rgb24")
{
	const std::array<std::uint8_t, 24> rgb{
		// clang-format off
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
		// clang-format on
	};

	const std::array<std::uint8_t, 24> bgr{
		// clang-format off
		0x02, 0x01, 0x00, 0x05, 0x04, 0x03, 0x08, 0x07, 0x06, 0x09, 0x0a, 0x0b,
		0x12, 0x11, 0x10, 0x15, 0x14, 0x13, 0x18, 0x17, 0x16, 0x19, 0x1a, 0x1b,
		// clang-format on
	};

	Image rgb_image{ { 3, 2, PixelFormat::Rgb24, 4 }, rgb.data() };
	REQUIRE(rgb_image.swap_channels());
	CHECK(!std::memcmp(rgb_image.data(), bgr.data(), bgr.size()));

	Image bgr_image{ { 3, 2, PixelFormat::Bgr24, 4 }, bgr.data() };
	REQUIRE(bgr_image.swap_channels());
	CHECK(!std::memcmp(bgr_image.data(), rgb.data(), rgb.size()));
}
