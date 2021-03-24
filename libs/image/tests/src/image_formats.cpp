// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "image_formats.h"

#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>
#include "image.h"

#include <doctest.h>

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

TEST_CASE("image.load_jpg24")
{
	const auto jpeg_image = Yt::Image::load(*Yt::Source::from("gradient24.jpg"));
	REQUIRE(jpeg_image);
	REQUIRE(jpeg_image->info().pixel_format() == Yt::PixelFormat::Bgra32);

	const auto tga_image = Yt::Image::load(*Yt::Source::from("gradient24.jpg.tga"));
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

TEST_CASE("image.save_jpg24.right_down")
{
	Yt::TemporaryFile file;
	REQUIRE(::make_test_image(false, Yt::ImageOrientation::XRightYDown).save(Yt::Writer{ file }, Yt::ImageFormat::Jpeg));
	CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient24.jpg")->to_buffer());
}

TEST_CASE("image.save_jpg24.right_up")
{
	Yt::TemporaryFile file;
	REQUIRE(::make_test_image(false, Yt::ImageOrientation::XRightYUp).save(Yt::Writer{ file }, Yt::ImageFormat::Jpeg));
	CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient24.jpg")->to_buffer());
}

TEST_CASE("image.save_png24.right_down")
{
	Yt::TemporaryFile file;
	REQUIRE(::make_test_image(false, Yt::ImageOrientation::XRightYDown).save(Yt::Writer{ file }, Yt::ImageFormat::Png));
	CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient24.png")->to_buffer());
}

TEST_CASE("image.save_png24.right_up")
{
	Yt::TemporaryFile file;
	REQUIRE(::make_test_image(false, Yt::ImageOrientation::XRightYUp).save(Yt::Writer{ file }, Yt::ImageFormat::Png));
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
