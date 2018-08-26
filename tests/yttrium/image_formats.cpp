#include "image_formats.h"

#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include "image.h"
#include "iostream.h"

#include <catch2/catch.hpp>

using Yttrium::Image;
using Yttrium::ImageType;
using Yttrium::PixelFormat;
using Yttrium::Source;
using Yttrium::TemporaryFile;

TEST_CASE("image.load.dds")
{
	const auto image = Image::load(*Source::from("tests/data/gradient32.dds"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(true));
}

#if Y_USE_JPEG
TEST_CASE("image.load.jpeg")
{
	const auto jpeg_image = Image::load(*Source::from("tests/data/gradient24.jpeg"));
	REQUIRE(jpeg_image);
	REQUIRE(jpeg_image->format().pixel_format() == PixelFormat::Rgb24);

	auto tga_image = Image::load(*Source::from("tests/data/gradient24.jpeg.tga"));
	REQUIRE(tga_image);
	REQUIRE(tga_image->format().pixel_format() == PixelFormat::Bgr24);
	REQUIRE(tga_image->swap_channels());

	CHECK(jpeg_image == tga_image);
}
#endif

TEST_CASE("image.load.tga")
{
	const auto image = Image::load(*Source::from("tests/data/gradient32.tga"));
	REQUIRE(image);
	CHECK(*image == ::make_test_image(true));
}

#if Y_USE_PNG
TEST_CASE("image.save.png")
{
	TemporaryFile file;
	REQUIRE(::make_test_image(false).save(file.name(), ImageType::Png));
	CHECK(Source::from(file)->to_buffer() == Source::from("tests/data/gradient24.png")->to_buffer());
}
#endif

TEST_CASE("image.save.tga")
{
	TemporaryFile file;
	REQUIRE(::make_test_image(true).save(file.name(), ImageType::Tga));
	CHECK(Source::from(file)->to_buffer() == Source::from("tests/data/gradient32.tga")->to_buffer());
}
