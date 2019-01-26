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

#include "image_formats.h"

#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>
#include "image.h"
#include "iostream.h"

#include <catch2/catch.hpp>

using Yttrium::Image;
using Yttrium::ImageType;
using Yttrium::PixelFormat;
using Yttrium::Source;
using Yttrium::TemporaryFile;
using Yttrium::Writer;

TEST_CASE("image.load.bmp")
{
	const auto image = Image::load(*Source::from("tests/core/data/gradient24.bmp"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(false));
}

TEST_CASE("image.load.dds")
{
	const auto image = Image::load(*Source::from("tests/core/data/gradient32.dds"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(true));
}

TEST_CASE("image.load.ico")
{
	const auto image = Image::load(*Source::from("tests/core/data/gradient32.ico"));
	REQUIRE(image);
	auto reference_image = ::make_test_image(true);
	reference_image.flip_vertically();
	CHECK(*image == reference_image);
}

#if Y_USE_JPEG
TEST_CASE("image.load.jpeg")
{
	const auto jpeg_image = Image::load(*Source::from("tests/core/data/gradient24.jpeg"));
	REQUIRE(jpeg_image);
	REQUIRE(jpeg_image->info().pixel_format() == PixelFormat::Rgb24);

	auto tga_image = Image::load(*Source::from("tests/core/data/gradient24.jpeg.tga"));
	REQUIRE(tga_image);
	REQUIRE(tga_image->info().pixel_format() == PixelFormat::Bgr24);
	REQUIRE(tga_image->swap_channels());

	CHECK(jpeg_image == tga_image);
}
#endif

TEST_CASE("image.load.tga8")
{
	const auto image = Image::load(*Source::from("tests/core/data/gradient8.tga"));
	REQUIRE(image);
	CHECK(*image == (::make_gray8_test_image<16>()));
}

TEST_CASE("image.load.tga24")
{
	const auto image = Image::load(*Source::from("tests/core/data/gradient24.tga"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(false));
}

TEST_CASE("image.load.tga32")
{
	const auto image = Image::load(*Source::from("tests/core/data/gradient32.tga"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(true));
}

#if Y_USE_PNG
TEST_CASE("image.save.png")
{
	TemporaryFile file;
	REQUIRE(::make_test_image(false).save(Writer{ file.name() }, ImageType::Png));
	CHECK(Source::from(file)->to_buffer() == Source::from("tests/core/data/gradient24.png")->to_buffer());
}
#endif

TEST_CASE("image.save.tga24")
{
	TemporaryFile file;
	REQUIRE(::make_test_image(false).save(Writer{ file.name() }, ImageType::Tga));
	CHECK(Source::from(file)->to_buffer() == Source::from("tests/core/data/gradient24.tga")->to_buffer());
}

TEST_CASE("image.save.tga32")
{
	TemporaryFile file;
	REQUIRE(::make_test_image(true).save(Writer{ file.name() }, ImageType::Tga));
	CHECK(Source::from(file)->to_buffer() == Source::from("tests/core/data/gradient32.tga")->to_buffer());
}
