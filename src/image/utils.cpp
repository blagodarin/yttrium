#include <yttrium/image.h>

#include <stdexcept>

namespace Yttrium
{
	Image grayscale_to_bgra(const Image& input)
	{
		Image output{{input.format().size(), PixelFormat::Bgra, 32, input.format().orientation()}};

		auto src = static_cast<const uint8_t*>(input.data());
		const auto src_row_size = input.format().row_size();

		auto dst = static_cast<uint8_t*>(output.data());
		const auto dst_row_size = output.format().row_size();

		const auto scanline_size = output.format().width() * 4;

		switch (input.format().pixel_format())
		{
		case PixelFormat::Gray:
			if (input.format().bits_per_pixel() == 8)
			{
				for (auto y = output.format().height(); y > 0; --y)
				{
					for (size_t a = 0, b = 0; a < scanline_size; a += 4, ++b)
					{
						dst[a + 0] = src[b + 0];
						dst[a + 1] = src[b + 0];
						dst[a + 2] = src[b + 0];
						dst[a + 3] = 0xff;
					}
					src += src_row_size;
					dst += dst_row_size;
				}
				return output;
			}
			break;

		case PixelFormat::GrayAlpha:
			if (input.format().bits_per_pixel() == 16)
			{
				for (auto y = output.format().height(); y > 0; --y)
				{
					for (size_t a = 0, b = 0; a < scanline_size; a += 4, b += 2)
					{
						dst[a + 0] = src[b + 0];
						dst[a + 1] = src[b + 0];
						dst[a + 2] = src[b + 0];
						dst[a + 3] = src[b + 1];
					}
					src += src_row_size;
					dst += dst_row_size;
				}
				return output;
			}
			break;

		default:
			break;
		}
		throw std::logic_error{"Bad image format for grayscale-BGRA conversion"};
	}

	boost::optional<Image> intensity_to_bgra(const Image& input)
	{
		if (input.format().pixel_format() != PixelFormat::Gray || input.format().bits_per_pixel() != 8)
			return {};

		Image output{{input.format().size(), PixelFormat::Bgra, 32, input.format().orientation()}};

		auto src = static_cast<const uint8_t*>(input.data());
		const auto src_row_size = input.format().row_size();

		auto dst = static_cast<uint8_t*>(output.data());
		const auto dst_row_size = output.format().row_size();

		const auto scanline_size = output.format().width() * 4;

		for (auto y = output.format().height(); y > 0; --y)
		{
			for (size_t a = 0, b = 0; a < scanline_size; a += 4, ++b)
			{
				dst[a + 0] = src[b + 0];
				dst[a + 1] = src[b + 0];
				dst[a + 2] = src[b + 0];
				dst[a + 3] = src[b + 0];
			}
			src += src_row_size;
			dst += dst_row_size;
		}

		return std::move(output);
	}
}
