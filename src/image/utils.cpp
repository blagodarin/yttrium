#include <yttrium/image.h>

#include <stdexcept>

namespace Yttrium
{
	Image to_bgra(const Image& input)
	{
		const auto input_format = input.format();
		if (input_format.pixel_format() == PixelFormat::Bgra32)
			return Image{input_format, input.data()};

		Image output{{input_format.size(), PixelFormat::Bgra32, input_format.orientation()}};

		auto src = static_cast<const uint8_t*>(input.data());
		const auto src_row_size = input_format.row_size();

		auto dst = static_cast<uint8_t*>(output.data());
		const auto dst_row_size = output.format().row_size();

		const auto scanline_size = output.format().width() * 4;

		switch (input_format.pixel_format())
		{
		case PixelFormat::Gray8:
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

		case PixelFormat::GrayAlpha16:
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

		case PixelFormat::Rgb24:
			for (auto y = output.format().height(); y > 0; --y)
			{
				for (size_t a = 0, b = 0; a < scanline_size; a += 4, b += 3)
				{
					dst[a + 0] = src[b + 2];
					dst[a + 1] = src[b + 1];
					dst[a + 2] = src[b + 0];
					dst[a + 3] = 0xff;
				}
				src += src_row_size;
				dst += dst_row_size;
			}
			return output;

		case PixelFormat::Bgr24:
			for (auto y = output.format().height(); y > 0; --y)
			{
				for (size_t a = 0, b = 0; a < scanline_size; a += 4, b += 3)
				{
					dst[a + 0] = src[b + 0];
					dst[a + 1] = src[b + 1];
					dst[a + 2] = src[b + 2];
					dst[a + 3] = 0xff;
				}
				src += src_row_size;
				dst += dst_row_size;
			}
			return output;

		case PixelFormat::Rgba32:
			for (auto y = output.format().height(); y > 0; --y)
			{
				for (size_t a = 0, b = 0; a < scanline_size; a += 4, b += 4)
				{
					dst[a + 0] = src[b + 2];
					dst[a + 1] = src[b + 1];
					dst[a + 2] = src[b + 0];
					dst[a + 3] = src[b + 3];
				}
				src += src_row_size;
				dst += dst_row_size;
			}
			return output;

		default:
			throw std::logic_error{"Bad image format for BGRA conversion"};
		}
	}

	std::optional<Image> intensity_to_bgra(const Image& input)
	{
		if (input.format().pixel_format() != PixelFormat::Gray8)
			return {};

		Image output{{input.format().size(), PixelFormat::Bgra32, input.format().orientation()}};

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
