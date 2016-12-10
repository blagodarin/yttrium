#include <yttrium/image.h>

#include <stdexcept>

namespace Yttrium
{
	Image grayscale_to_bgra(const Image& input) // TODO: Tests.
	{
		const auto input_format = input.format();

		auto output_format = input_format;
		output_format.set_pixel_format(PixelFormat::Bgra, 32);

		Image output(output_format);

		auto src = static_cast<const uint8_t*>(input.data());
		auto dst = static_cast<uint8_t*>(output.data());
		const auto scanline_size = output_format.width() * 4;

		switch (input_format.pixel_format())
		{
		case PixelFormat::Gray:
			if (input_format.bits_per_pixel() == 8)
			{
				for (size_t y = 0; y < output_format.height(); ++y)
				{
					for (size_t a = 0, b = 0; a < scanline_size; a += 4, ++b)
					{
						dst[a + 0] = src[b + 0];
						dst[a + 1] = src[b + 0];
						dst[a + 2] = src[b + 0];
						dst[a + 3] = 0xff;
					}
					src += input_format.row_size();
					dst += output_format.row_size();
				}
				return output;
			}
			break;

		case PixelFormat::GrayAlpha:
			if (input_format.bits_per_pixel() == 16)
			{
				for (size_t y = 0; y < output_format.height(); ++y)
				{
					for (size_t a = 0, b = 0; a < scanline_size; a += 4, b += 2)
					{
						dst[a + 0] = src[b + 0];
						dst[a + 1] = src[b + 0];
						dst[a + 2] = src[b + 0];
						dst[a + 3] = src[b + 1];
					}
					src += input_format.row_size();
					dst += output_format.row_size();
				}
				return output;
			}
			break;

		default:
			break;
		}
		throw std::logic_error("Bad image format for grayscale-BGRA conversion");
	}
}
