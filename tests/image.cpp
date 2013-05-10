#include <Yttrium/buffer.h>
#include <Yttrium/file.h>
#include <Yttrium/image.h>
#include <Yttrium/memory_manager.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

// The TGA test must come first because the TGA format
// is the most basic one for both loading and saving.

BOOST_AUTO_TEST_CASE(tga_test)
{
	MemoryManager memory_manager;

	Image image;

	BOOST_REQUIRE(image.load("tests/image/image.tga"));

	File file(File::Temporary);

	BOOST_REQUIRE(image.save(file.name(), ImageType::Tga));

	Buffer expected;
	Buffer actual;

	BOOST_REQUIRE(File("tests/image/image.tga").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}

BOOST_AUTO_TEST_CASE(jpeg_test)
{
	MemoryManager memory_manager;

	Image jpeg_image;

	BOOST_REQUIRE(jpeg_image.load("tests/image/jpeg.jpeg"));
	BOOST_REQUIRE(jpeg_image.format().pixel_format() == PixelFormat::Rgb);

	Image tga_image;

	BOOST_REQUIRE(tga_image.load("tests/image/jpeg.tga"));
	BOOST_REQUIRE(tga_image.format().pixel_format() == PixelFormat::Bgr);
	BOOST_REQUIRE(tga_image.swap_channels());

	BOOST_CHECK(jpeg_image == tga_image);
}

BOOST_AUTO_TEST_CASE(png_test)
{
	MemoryManager memory_manager;

	Image image;

	BOOST_REQUIRE(image.load("tests/image/image.tga"));

	File file(File::Temporary);

	BOOST_REQUIRE(image.save(file.name(), ImageType::Png));

	Buffer expected;
	Buffer actual;

	BOOST_REQUIRE(File("tests/image/image.png").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}

BOOST_AUTO_TEST_CASE(intensity_test)
{
	MemoryManager memory_manager;

	Image image;

	BOOST_REQUIRE(image.load("tests/image/gradient8.tga"));
	BOOST_REQUIRE(image.intensity_to_bgra());

	File file(File::Temporary);

	BOOST_REQUIRE(image.save(file.name(), ImageType::Tga));

	Buffer expected;
	Buffer actual;

	BOOST_REQUIRE(File("tests/image/intensity32.tga").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}
