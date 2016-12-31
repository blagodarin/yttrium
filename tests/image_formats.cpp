#include "image.h"

#include <yttrium/memory/buffer.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/temporary_file.h>
#include "src/config.h"

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
	const auto image = Image::load(Reader("tests/image/gradient32.tga"));
	BOOST_REQUIRE(image);

	TemporaryFile file;
	BOOST_REQUIRE(image->save(file.name(), ImageType::Tga));

	const auto expected = Reader("tests/image/gradient32.tga").to_buffer();
	const auto actual = Reader(file).to_buffer();
	BOOST_CHECK_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_CASE(test_dds)
{
	const auto dds_image = Image::load(Reader("tests/image/gradient32.dds"));
	BOOST_REQUIRE(dds_image);

	const auto tga_image = Image::load(Reader("tests/image/gradient32.tga"));
	BOOST_REQUIRE(tga_image);

	BOOST_CHECK(dds_image == tga_image);
}

#ifndef Y_NO_JPEG
BOOST_AUTO_TEST_CASE(test_jpeg)
{
	const auto jpeg_image = Image::load(Reader("tests/image/gradient24.jpeg"));
	BOOST_REQUIRE(jpeg_image);
	BOOST_REQUIRE(jpeg_image->format().pixel_format() == PixelFormat::Rgb);

	auto tga_image = Image::load(Reader("tests/image/gradient24.jpeg.tga"));
	BOOST_REQUIRE(tga_image);
	BOOST_REQUIRE(tga_image->format().pixel_format() == PixelFormat::Bgr);
	BOOST_REQUIRE(tga_image->swap_channels());

	BOOST_CHECK(jpeg_image == tga_image);
}
#endif

#ifndef Y_NO_PNG
BOOST_AUTO_TEST_CASE(test_png)
{
	const auto image = Image::load(Reader("tests/image/gradient24.tga"));
	BOOST_REQUIRE(image);

	TemporaryFile file;
	BOOST_REQUIRE(image->save(file.name(), ImageType::Png));

	const auto expected = Reader("tests/image/gradient24.png").to_buffer();
	const auto actual = Reader(file).to_buffer();
	BOOST_CHECK_EQUAL(expected, actual);
}
#endif
