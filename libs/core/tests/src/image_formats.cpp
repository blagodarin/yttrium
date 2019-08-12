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

#include "image_formats.h"

#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>
#include "image.h"
#include "iostream.h"

#include <catch2/catch.hpp>

TEST_CASE("image.load_bmp24")
{
	const auto image = Yt::Image::load(*Yt::Source::from("gradient24.bmp"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(false));
}

TEST_CASE("image.load_dds32")
{
	const auto image = Yt::Image::load(*Yt::Source::from("gradient32.dds"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(true));
}

TEST_CASE("image.load_ico32")
{
	const auto image = Yt::Image::load(*Yt::Source::from("gradient32.ico"));
	REQUIRE(image);

	const auto reference_image = ::make_test_image(true, Yt::ImageOrientation::XRightYUp);
	CHECK(*image == reference_image);
}

TEST_CASE("image.load_jpeg24")
{
	const auto jpeg_image = Yt::Image::load(*Yt::Source::from("jpeg24.jpeg"));
	REQUIRE(jpeg_image);
	REQUIRE(jpeg_image->info().pixel_format() == Yt::PixelFormat::Bgra32);

	const auto tga_image = Yt::Image::load(*Yt::Source::from("jpeg24.tga"));
	REQUIRE(tga_image);
	REQUIRE(tga_image->info().pixel_format() == Yt::PixelFormat::Bgra32);

	CHECK(jpeg_image == tga_image);
}

TEST_CASE("image.load_tga8")
{
	const auto image = Yt::Image::load(*Yt::Source::from("gradient8.tga"));
	REQUIRE(image);
	CHECK(*image == (::make_gray8_test_image<16>()));
}

TEST_CASE("image.load_tga24")
{
	const auto image = Yt::Image::load(*Yt::Source::from("gradient24.tga"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(false));
}

TEST_CASE("image.load_tga32")
{
	const auto image = Yt::Image::load(*Yt::Source::from("gradient32.tga"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(true));
}

TEST_CASE("image.save_png24")
{
	Yt::TemporaryFile file;
	REQUIRE(::make_test_image(false).save(Yt::Writer{ file }, Yt::ImageFormat::Png));
	CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient24.png")->to_buffer());
}

TEST_CASE("image.save_tga24")
{
	Yt::TemporaryFile file;
	REQUIRE(::make_test_image(false).save(Yt::Writer{ file }, Yt::ImageFormat::Tga));
	CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient24.tga")->to_buffer());
}

TEST_CASE("image.save_tga32")
{
	Yt::TemporaryFile file;
	REQUIRE(::make_test_image(true).save(Yt::Writer{ file }, Yt::ImageFormat::Tga));
	CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient32.tga")->to_buffer());
}
