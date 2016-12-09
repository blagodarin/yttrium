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
		const auto src_padding = input_format.row_size() - input_format.width() * 2;
		const auto dst_padding = output_format.row_size() - output_format.width() * 4;

		switch (input_format.pixel_format())
		{
		case PixelFormat::Gray:
			if (input_format.bits_per_pixel() == 8)
			{
				for (size_t i = 0; i < output_format.height(); ++i)
				{
					for (size_t j = 0; j < output_format.width(); ++j)
					{
						dst[0] = src[0];
						dst[1] = src[0];
						dst[2] = src[0];
						dst[3] = 0xff;
						src += 1;
						dst += 4;
					}
					src += src_padding;
					dst += dst_padding;
				}
				return output;
			}
			break;

		case PixelFormat::GrayAlpha:
			if (input_format.bits_per_pixel() == 16)
			{
				for (size_t i = 0; i < output_format.height(); ++i)
				{
					for (size_t j = 0; j < output_format.width(); ++j)
					{
						dst[0] = src[0];
						dst[1] = src[0];
						dst[2] = src[0];
						dst[3] = src[1];
						src += 2;
						dst += 4;
					}
					src += src_padding;
					dst += dst_padding;
				}
				return output;
			}
			break;

		case PixelFormat::AlphaGray:
			if (input_format.bits_per_pixel() == 16)
			{
				for (size_t i = 0; i < output_format.height(); ++i)
				{
					for (size_t j = 0; j < output_format.width(); ++j)
					{
						dst[0] = src[1];
						dst[1] = src[1];
						dst[2] = src[1];
						dst[3] = src[0];
						src += 2;
						dst += 4;
					}
					src += src_padding;
					dst += dst_padding;
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
