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

#include "image.h"

#include "../../../libs/core/src/image/utils.h"

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

TEST_CASE("image.convert.jpeg420_to_bgra")
{
	const std::array<std::uint8_t, 6> jpeg420{
		0x00, 0x55,
		0xaa, 0xff,
		0x80,
		0x80
	};

	const std::array<std::uint8_t, 16> bgra{
		0x00, 0x00, 0x00, 0xff, 0x55, 0x55, 0x55, 0xff,
		0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff, 0xff
	};

	Yttrium::YCbCrComponents components;
	components.y = jpeg420.data();
	components.y_stride = 2;
	components.cb = components.y + components.y_stride * 2;
	components.cbcr_stride = 1;
	components.cr = components.cb + components.cbcr_stride;

	Yttrium::Buffer output{ bgra.size() };
	std::memset(output.data(), 0xee, output.size());
	Yttrium::convert_jpeg420_to_bgra(2, 2, components, output.data(), 8);
	CHECK(std::memcmp(output.data(), bgra.data(), bgra.size()) == 0);
}

TEST_CASE("image.transform.bgr_to_bgra")
{
	const std::array<std::uint8_t, 24> bgr{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b
	};

	const std::array<std::uint8_t, 24> bgra{
		0x00, 0x01, 0x02, 0xff, 0x03, 0x04, 0x05, 0xff, 0x06, 0x07, 0x08, 0xff,
		0x10, 0x11, 0x12, 0xff, 0x13, 0x14, 0x15, 0xff, 0x16, 0x17, 0x18, 0xff
	};

	const Image input{ { 3, 2, PixelFormat::Bgr24, 4 }, bgr.data() };
	Image output{ { 3, 2, PixelFormat::Bgra32 } };
	REQUIRE(Image::transform(input.info(), input.data(), output.info(), output.data()));

	const Image expected{ output.info(), bgra.data() };
	CHECK(output == expected);
}

TEST_CASE("image.transform.bgra_to_bgra")
{
	const std::array<std::uint8_t, 24> bgra{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b
	};

	const Image input{ { 3, 2, PixelFormat::Bgra32 }, bgra.data() };
	Image output{ { 3, 2, PixelFormat::Bgra32 } };
	REQUIRE(Image::transform(input.info(), input.data(), output.info(), output.data()));

	const Image expected{ output.info(), bgra.data() };
	CHECK(output == expected);
}

TEST_CASE("image.transform.gray_to_bgra")
{
	const std::array<std::uint8_t, 8> gray{
		0x00, 0x01, 0x02, 0x03,
		0x10, 0x11, 0x12, 0x13
	};

	const std::array<std::uint8_t, 24> bgra{
		0x00, 0x00, 0x00, 0xff, 0x01, 0x01, 0x01, 0xff, 0x02, 0x02, 0x02, 0xff,
		0x10, 0x10, 0x10, 0xff, 0x11, 0x11, 0x11, 0xff, 0x12, 0x12, 0x12, 0xff
	};

	const Image input{ { 3, 2, PixelFormat::Gray8, 4 }, gray.data() };
	Image output{ { 3, 2, PixelFormat::Bgra32 } };
	REQUIRE(Image::transform(input.info(), input.data(), output.info(), output.data()));

	const Image expected{ output.info(), bgra.data() };
	CHECK(output == expected);
}

TEST_CASE("image.transform.gray_alpha_to_bgra")
{
	const std::array<std::uint8_t, 16> gray_alpha{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
	};

	const std::array<std::uint8_t, 24> bgra{
		0x00, 0x00, 0x00, 0x01, 0x02, 0x02, 0x02, 0x03, 0x04, 0x04, 0x04, 0x05,
		0x10, 0x10, 0x10, 0x11, 0x12, 0x12, 0x12, 0x13, 0x14, 0x14, 0x14, 0x15
	};

	const Image input{ { 3, 2, PixelFormat::GrayAlpha16, 4 }, gray_alpha.data() };
	Image output{ { 3, 2, PixelFormat::Bgra32 } };
	REQUIRE(Image::transform(input.info(), input.data(), output.info(), output.data()));

	const Image expected{ output.info(), bgra.data() };
	CHECK(output == expected);
}

TEST_CASE("image.transform.intensity_to_bgra")
{
	const std::array<std::uint8_t, 8> intensity{
		0x00, 0x01, 0x02, 0x03,
		0x10, 0x11, 0x12, 0x13
	};

	const std::array<std::uint8_t, 24> bgra{
		0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02,
		0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12
	};

	const Image input{ { 3, 2, PixelFormat::Intensity8, 4 }, intensity.data() };
	Image output{ { 3, 2, PixelFormat::Bgra32 } };
	REQUIRE(Image::transform(input.info(), input.data(), output.info(), output.data()));

	const Image expected{ output.info(), bgra.data() };
	CHECK(output == expected);
}

TEST_CASE("image.transform.rgb_to_bgra")
{
	const std::array<std::uint8_t, 24> rgb{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b
	};

	const std::array<std::uint8_t, 24> bgra{
		0x02, 0x01, 0x00, 0xff, 0x05, 0x04, 0x03, 0xff, 0x08, 0x07, 0x06, 0xff,
		0x12, 0x11, 0x10, 0xff, 0x15, 0x14, 0x13, 0xff, 0x18, 0x17, 0x16, 0xff
	};

	const Image input{ { 3, 2, PixelFormat::Rgb24, 4 }, rgb.data() };
	Image output{ { 3, 2, PixelFormat::Bgra32 } };
	REQUIRE(Image::transform(input.info(), input.data(), output.info(), output.data()));

	const Image expected{ output.info(), bgra.data() };
	CHECK(output == expected);
}

TEST_CASE("image.transform.rgba_to_bgra")
{
	const std::array<std::uint8_t, 24> rgba{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b
	};

	const std::array<uint8_t, 24> bgra{
		0x02, 0x01, 0x00, 0x03, 0x06, 0x05, 0x04, 0x07, 0x0a, 0x09, 0x08, 0x0b,
		0x12, 0x11, 0x10, 0x13, 0x16, 0x15, 0x14, 0x17, 0x1a, 0x19, 0x18, 0x1b
	};

	const Image input{ { 3, 2, PixelFormat::Rgba32 }, rgba.data() };
	Image output{ { 3, 2, PixelFormat::Bgra32 } };
	REQUIRE(Image::transform(input.info(), input.data(), output.info(), output.data()));

	const Image expected{ output.info(), bgra.data() };
	CHECK(output == expected);
}
