#include <yttrium/file.h>
#include <yttrium/image.h>
#include <yttrium/memory/buffer.h>

#include <boost/test/unit_test.hpp>

namespace Yttrium
{
	inline std::ostream& operator<<(std::ostream& stream, const Buffer& buffer)
	{
		return stream << "Buffer(" << buffer.size() << ")";
	}
}

using namespace Yttrium;

// The TGA test must come first because the TGA format
// is the most basic one for both loading and saving.

BOOST_AUTO_TEST_CASE(test_tga)
{
	Image image;
	BOOST_REQUIRE(image.load("tests/image/gradient32.tga"));

	File file(File::Temporary);
	BOOST_REQUIRE(image.save(file.name(), ImageType::Tga));

	Buffer expected;
	Buffer actual;
	BOOST_REQUIRE(File("tests/image/gradient32.tga").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}

BOOST_AUTO_TEST_CASE(test_dds)
{
	Image dds_image;
	BOOST_REQUIRE(dds_image.load("tests/image/gradient32.dds"));

	Image tga_image;
	BOOST_REQUIRE(tga_image.load("tests/image/gradient32.tga"));

	BOOST_CHECK(dds_image == tga_image);
}

BOOST_AUTO_TEST_CASE(test_jpeg)
{
	Image jpeg_image;
	BOOST_REQUIRE(jpeg_image.load("tests/image/gradient24.jpeg"));
	BOOST_REQUIRE(jpeg_image.format().pixel_format() == PixelFormat::Rgb);

	Image tga_image;
	BOOST_REQUIRE(tga_image.load("tests/image/gradient24.jpeg.tga"));
	BOOST_REQUIRE(tga_image.format().pixel_format() == PixelFormat::Bgr);
	BOOST_REQUIRE(tga_image.swap_channels());

	BOOST_CHECK(jpeg_image == tga_image);
}

BOOST_AUTO_TEST_CASE(test_png)
{
	Image image;
	BOOST_REQUIRE(image.load("tests/image/gradient24.tga"));

	File file(File::Temporary);
	BOOST_REQUIRE(image.save(file.name(), ImageType::Png));

	const auto expected = File::read_to_buffer("tests/image/gradient24.png");
	BOOST_REQUIRE(expected.size() > 0);
	const auto actual = File::read_to_buffer(file.name());
	BOOST_CHECK_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_CASE(test_intensity)
{
	Image image;
	BOOST_REQUIRE(image.load("tests/image/intensity8.tga"));
	BOOST_REQUIRE(image.intensity_to_bgra());

	File file(File::Temporary);
	BOOST_REQUIRE(image.save(file.name(), ImageType::Tga));

	Buffer expected;
	Buffer actual;
	BOOST_REQUIRE(File("tests/image/intensity32.tga").read_all(&expected));
	BOOST_REQUIRE(File(file.name()).read_all(&actual));
	BOOST_CHECK(expected == actual);
}
