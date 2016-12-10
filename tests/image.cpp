#include <yttrium/image.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/temporary_file.h>
#include "src/config.h"
#include "src/image/utils.h"

#include <boost/test/unit_test.hpp>

namespace Yttrium
{
	inline std::ostream& operator<<(std::ostream& stream, const Buffer& buffer)
	{
		return stream << "Buffer(" << buffer.size() << ")";
	}

	inline bool operator==(const Image& a, const Image& b)
	{
		const auto format = a.format();
		if (format != b.format())
			return false;
		for (size_t i = 0; i < format.height(); ++i)
		{
			const auto a_row = static_cast<const uint8_t*>(a.data()) + i * format.row_size();
			const auto b_row = static_cast<const uint8_t*>(b.data()) + i * format.row_size();
			if (std::memcmp(a_row, b_row, unaligned_image_row_size(format.width(), format.bits_per_pixel())))
				return false;
		}
		return true;
	}
}

using namespace Yttrium;

// The TGA test must come first because the TGA format
// is the most basic one for both loading and saving.

BOOST_AUTO_TEST_CASE(test_tga)
{
	Image image;
	BOOST_REQUIRE(image.load(Reader("tests/image/gradient32.tga")));

	TemporaryFile file;
	BOOST_REQUIRE(image.save(file.name(), ImageType::Tga));

	const auto expected = Reader("tests/image/gradient32.tga").to_buffer();
	const auto actual = Reader(file).to_buffer();
	BOOST_CHECK_EQUAL(expected, actual);
}

BOOST_AUTO_TEST_CASE(test_dds)
{
	Image dds_image;
	BOOST_REQUIRE(dds_image.load(Reader("tests/image/gradient32.dds")));

	Image tga_image;
	BOOST_REQUIRE(tga_image.load(Reader("tests/image/gradient32.tga")));

	BOOST_CHECK(dds_image == tga_image);
}

BOOST_AUTO_TEST_CASE(test_grayscale_to_bgra)
{
	Image y8;
	BOOST_REQUIRE(y8.load(Reader("tests/image/y8.tga")));

	Image bgrx32;
	BOOST_REQUIRE(bgrx32.load(Reader("tests/image/y8_bgrx32.tga")));

	BOOST_CHECK(grayscale_to_bgra(y8) == bgrx32);
}

#ifndef Y_NO_JPEG
BOOST_AUTO_TEST_CASE(test_jpeg)
{
	Image jpeg_image;
	BOOST_REQUIRE(jpeg_image.load(Reader("tests/image/gradient24.jpeg")));
	BOOST_REQUIRE(jpeg_image.format().pixel_format() == PixelFormat::Rgb);

	Image tga_image;
	BOOST_REQUIRE(tga_image.load(Reader("tests/image/gradient24.jpeg.tga")));
	BOOST_REQUIRE(tga_image.format().pixel_format() == PixelFormat::Bgr);
	BOOST_REQUIRE(tga_image.swap_channels());

	BOOST_CHECK(jpeg_image == tga_image);
}
#endif

#ifndef Y_NO_PNG
BOOST_AUTO_TEST_CASE(test_png)
{
	Image image;
	BOOST_REQUIRE(image.load(Reader("tests/image/gradient24.tga")));

	TemporaryFile file;
	BOOST_REQUIRE(image.save(file.name(), ImageType::Png));

	const auto expected = Reader("tests/image/gradient24.png").to_buffer();
	const auto actual = Reader(file).to_buffer();
	BOOST_CHECK_EQUAL(expected, actual);
}
#endif

BOOST_AUTO_TEST_CASE(test_intensity)
{
	Image actual;
	BOOST_REQUIRE(actual.load(Reader("tests/image/y8.tga")));
	BOOST_REQUIRE(actual.intensity_to_bgra());

	Image expected;
	BOOST_REQUIRE(expected.load(Reader("tests/image/y8_bgra32.tga")));

	BOOST_CHECK(expected == actual);
}
