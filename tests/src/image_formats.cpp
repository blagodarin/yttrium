#include "image_formats.h"

#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include "../../src/config.h"
#include "image.h"
#include "iostream.h"

#include <boost/test/unit_test.hpp>

using Yttrium::Image;
using Yttrium::ImageType;
using Yttrium::PixelFormat;
using Yttrium::Source;
using Yttrium::TemporaryFile;

BOOST_AUTO_TEST_CASE(test_dds_reading)
{
	const auto image = Image::load(*Source::from("tests/data/gradient32.dds"));
	BOOST_REQUIRE(image);
	BOOST_CHECK(*image == ::make_test_image(true));
}

#ifndef Y_NO_JPEG
BOOST_AUTO_TEST_CASE(test_jpeg_reading)
{
	const auto jpeg_image = Image::load(*Source::from("tests/data/gradient24.jpeg"));
	BOOST_REQUIRE(jpeg_image);
	BOOST_REQUIRE(jpeg_image->format().pixel_format() == PixelFormat::Rgb24);

	auto tga_image = Image::load(*Source::from("tests/data/gradient24.jpeg.tga"));
	BOOST_REQUIRE(tga_image);
	BOOST_REQUIRE(tga_image->format().pixel_format() == PixelFormat::Bgr24);
	BOOST_REQUIRE(tga_image->swap_channels());

	BOOST_CHECK(jpeg_image == tga_image);
}
#endif

#ifndef Y_NO_PNG
BOOST_AUTO_TEST_CASE(test_png_writing)
{
	TemporaryFile file;
	BOOST_REQUIRE(::make_test_image(false).save(file.name(), ImageType::Png));
	const auto expected = Source::from("tests/data/gradient24.png")->to_buffer();
	const auto actual = Source::from(file)->to_buffer();
	BOOST_CHECK_EQUAL(expected, actual);
}
#endif

BOOST_AUTO_TEST_CASE(test_tga_reading)
{
	const auto image = Image::load(*Source::from("tests/data/gradient32.tga"));
	BOOST_REQUIRE(image);
	BOOST_CHECK(*image == ::make_test_image(true));
}

BOOST_AUTO_TEST_CASE(test_tga_writing)
{
	TemporaryFile file;
	BOOST_REQUIRE(::make_test_image(true).save(file.name(), ImageType::Tga));
	const auto expected = Source::from("tests/data/gradient32.tga")->to_buffer();
	const auto actual = Source::from(file)->to_buffer();
	BOOST_CHECK_EQUAL(expected, actual);
}
