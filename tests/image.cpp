#include "image.h"

#include <array>

#include <boost/test/unit_test.hpp>

using Yttrium::Image;
using Yttrium::PixelFormat;

BOOST_AUTO_TEST_CASE(test_image_comparison)
{
	{
		const uint8_t data = 0x01;
		const Image image1{ { 1, 1, PixelFormat::Gray, 8 }, &data };
		const Image image2{ { 1, 1, PixelFormat::Gray, 8 }, &data };
		BOOST_CHECK(image1 == image2);
	}
	{
		const uint8_t data1 = 0x01;
		const uint8_t data2 = 0x02;
		const Image image1{ { 1, 1, PixelFormat::Gray, 8 }, &data1 };
		const Image image2{ { 1, 1, PixelFormat::Gray, 8 }, &data2 };
		BOOST_CHECK(image1 != image2);
	}
	{
		const std::array<uint8_t, 2> data = { 0x01, 0x02 };
		const Image image1{ { 2, 1, PixelFormat::Gray, 8 }, data.data() };
		const Image image2{ { 1, 2, PixelFormat::Gray, 8 }, data.data() };
		BOOST_CHECK(image1 != image2);
	}
}

BOOST_AUTO_TEST_CASE(test_gray_alpha_to_bgra)
{
	const std::array<uint8_t, 16> gray_alpha =
	{
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
		0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
	};

	const std::array<uint8_t, 24> bgra =
	{
		0x01, 0x01, 0x01, 0x23, 0x45, 0x45, 0x45, 0x67, 0x89, 0x89, 0x89, 0xab,
		0xfe, 0xfe, 0xfe, 0xdc, 0xba, 0xba, 0xba, 0x98, 0x76, 0x76, 0x76, 0x54,
	};

	const Image gray_alpha_image({ 3, 2, PixelFormat::GrayAlpha, 16, 4 }, gray_alpha.data());
	const Image bgra_image({ 3, 2, PixelFormat::Bgra, 32 }, bgra.data());

	BOOST_CHECK(grayscale_to_bgra(gray_alpha_image) == bgra_image);
}

BOOST_AUTO_TEST_CASE(test_gray_to_bgra)
{
	const std::array<uint8_t, 8> gray =
	{
		0x01, 0x23, 0x45, 0x67,
		0x89, 0xab, 0xcd, 0xef,
	};

	const std::array<uint8_t, 24> bgra =
	{
		0x01, 0x01, 0x01, 0xff, 0x23, 0x23, 0x23, 0xff, 0x45, 0x45, 0x45, 0xff,
		0x89, 0x89, 0x89, 0xff, 0xab, 0xab, 0xab, 0xff, 0xcd, 0xcd, 0xcd, 0xff,
	};

	const Image gray_image({ 3, 2, PixelFormat::Gray, 8, 4 }, gray.data());
	const Image bgra_image({ 3, 2, PixelFormat::Bgra, 32 }, bgra.data());

	BOOST_CHECK(grayscale_to_bgra(gray_image) == bgra_image);
}

BOOST_AUTO_TEST_CASE(test_intensity_to_bgra)
{
	const std::array<uint8_t, 8> intensity =
	{
		0x01, 0x23, 0x45, 0x67,
		0x89, 0xab, 0xcd, 0xef,
	};

	const std::array<uint8_t, 24> bgra =
	{
		0x01, 0x01, 0x01, 0x01, 0x23, 0x23, 0x23, 0x23, 0x45, 0x45, 0x45, 0x45,
		0x89, 0x89, 0x89, 0x89, 0xab, 0xab, 0xab, 0xab, 0xcd, 0xcd, 0xcd, 0xcd,
	};

	const Image intensity_image({ 3, 2, PixelFormat::Gray, 8, 4 }, intensity.data());
	const Image bgra_image({ 3, 2, PixelFormat::Bgra, 32 }, bgra.data());

	BOOST_CHECK(*intensity_to_bgra(intensity_image) == bgra_image);
}
