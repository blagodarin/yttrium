// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "formats.h"

#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary.h>
#include <yttrium/storage/writer.h>
#include "image.h"

#include <doctest/doctest.h>

#if YTTRIUM_IMAGE_BMP
TEST_CASE("BMP.Load.24")
{
	const auto image = Yt::Image::load(*Yt::Source::from("gradient24.bmp"));
	REQUIRE(image);
	CHECK(*image == ::makeTestImage(false));
}
#endif

#if YTTRIUM_IMAGE_DDS
TEST_CASE("DDS.Load.32")
{
	const auto image = Yt::Image::load(*Yt::Source::from("gradient32.dds"));
	REQUIRE(image);
	CHECK(*image == ::makeTestImage(true));
}
#endif

#if YTTRIUM_IMAGE_ICO
TEST_CASE("ICO.Load.32")
{
	const auto image = Yt::Image::load(*Yt::Source::from("gradient32.ico"));
	REQUIRE(image);
	CHECK(*image == ::makeTestImage(true, Yt::ImageOrientation::XRightYUp));
}
#endif

#if YTTRIUM_IMAGE_JPEG
TEST_CASE("JPEG")
{
#	if YTTRIUM_IMAGE_TGA
	SUBCASE("Load.24")
	{
		const auto jpeg_image = Yt::Image::load(*Yt::Source::from("gradient24.jpg"));
		REQUIRE(jpeg_image);
		REQUIRE(jpeg_image->info().pixel_format() == Yt::PixelFormat::Bgra32);

		const auto tga_image = Yt::Image::load(*Yt::Source::from("gradient24.jpg.tga"));
		REQUIRE(tga_image);
		REQUIRE(tga_image->info().pixel_format() == Yt::PixelFormat::Bgra32);

		CHECK(jpeg_image == tga_image);
	}
#	endif
	SUBCASE("Save.24")
	{
		Yt::TemporaryFile file;
		SUBCASE("XRightYDown")
		{
			REQUIRE(::makeTestImage(false, Yt::ImageOrientation::XRightYDown).save(Yt::Writer{ file }, Yt::ImageFormat::Jpeg));
		}
		SUBCASE("XRightYUp")
		{
			REQUIRE(::makeTestImage(false, Yt::ImageOrientation::XRightYUp).save(Yt::Writer{ file }, Yt::ImageFormat::Jpeg));
		}
		CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient24.jpg")->to_buffer());
	}
}
#endif

#if YTTRIUM_IMAGE_PNG
TEST_CASE("PNG.Save.24")
{
	Yt::TemporaryFile file;
	SUBCASE("XRightYDown")
	{
		REQUIRE(::makeTestImage(false, Yt::ImageOrientation::XRightYDown).save(Yt::Writer{ file }, Yt::ImageFormat::Png));
	}
	SUBCASE("XRightYUp")
	{
		REQUIRE(::makeTestImage(false, Yt::ImageOrientation::XRightYUp).save(Yt::Writer{ file }, Yt::ImageFormat::Png));
	}
	CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient24.png")->to_buffer());
}
#endif

#if YTTRIUM_IMAGE_TGA
TEST_CASE("TGA")
{
	SUBCASE("Load")
	{
		SUBCASE("8")
		{
			const auto image = Yt::Image::load(*Yt::Source::from("gradient8.tga"));
			REQUIRE(image);
			CHECK(*image == (::makeTestImageGray8<16>()));
		}
		SUBCASE("24")
		{
			const auto image = Yt::Image::load(*Yt::Source::from("gradient24.tga"));
			REQUIRE(image);
			CHECK(*image == ::makeTestImage(false));
		}
		SUBCASE("32")
		{
			const auto image = Yt::Image::load(*Yt::Source::from("gradient32.tga"));
			REQUIRE(image);
			CHECK(*image == ::makeTestImage(true));
		}
	}
	SUBCASE("Save")
	{
		Yt::TemporaryFile file;
		SUBCASE("24")
		{
			REQUIRE(::makeTestImage(false).save(Yt::Writer{ file }, Yt::ImageFormat::Tga));
			CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient24.tga")->to_buffer());
		}
		SUBCASE("32")
		{
			REQUIRE(::makeTestImage(true).save(Yt::Writer{ file }, Yt::ImageFormat::Tga));
			CHECK(Yt::Source::from(file)->to_buffer() == Yt::Source::from("gradient32.tga")->to_buffer());
		}
	}
}
#endif
