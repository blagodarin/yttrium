#include <yttrium/file.h>
#include <yttrium/memory/buffer.h>
#include "../../src/image/dds_private.h"
#include "../../src/image/tga_private.h"

#include <cstring>

using namespace Yttrium;

void write_color_gradient(File& file, bool with_alpha)
{
	Buffer buffer(16 * 16 * (with_alpha ? 4 : 3));

	auto data = &buffer[0];

	for (size_t y = 0; y < 16; ++y)
	{
		for (size_t x = 0; x < 16; ++x)
		{
			uint8_t b = 0;
			uint8_t g = 0;
			uint8_t r = 0;

			if (y < 4)
			{
				r = (y * 16 + x) * 4 + 3;
			}
			else if (y < 8)
			{
				r = ((7 - y) * 16 + x) * 4 + 3;
				g = ((y - 4) * 16 + x) * 4 + 3;
			}
			else if (y < 12)
			{
				g = ((11 - y) * 16 + x) * 4 + 3;
				b = (( y - 8) * 16 + x) * 4 + 3;
			}
			else
			{
				b = ((15 - y) * 16 + x) * 4 + 3;
			}

			*data++ = b;
			*data++ = g;
			*data++ = r;

			if (with_alpha)
				*data++ = x * 16 + 15;
		}
	}

	file.write(buffer.data(), buffer.size());
}

void write_color_intensity(File& file)
{
	Buffer buffer(16 * 16 * 4);

	auto data = &buffer[0];

	for (size_t y = 0; y < 16; ++y)
	{
		for (size_t x = 0; x < 16; ++x)
		{
			uint8_t value = y * 16 + x;

			*data++ = value;
			*data++ = value;
			*data++ = value;
			*data++ = value;
		}
	}

	file.write(buffer.data(), buffer.size());
}

void write_grayscale_intensity(File& file)
{
	Buffer buffer(16 * 16);

	auto data = &buffer[0];

	for (size_t y = 0; y < 16; ++y)
		for (size_t x = 0; x < 16; ++x)
			*data++ = y * 16 + x;

	file.write(buffer.data(), buffer.size());
}

int main(int, char**)
{
	{
		File file("tests/image/gradient24.tga", File::Write | File::Truncate);

		TgaHeader header;

		::memset(&header, 0, sizeof(header));

		header.image_type = tgaTrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 24;
		header.image.descriptor = tgaTopLeft;

		if (file.write(header))
			write_color_gradient(file, false);
	}
	{
		File file("tests/image/gradient32.tga", File::Write | File::Truncate);

		TgaHeader header;

		::memset(&header, 0, sizeof(header));

		header.image_type = tgaTrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 32;
		header.image.descriptor = tgaTopLeft | 8;

		if (file.write(header))
			write_color_gradient(file, true);
	}
	{
		File file("tests/image/gradient32.dds", File::Write | File::Truncate);

		DDS_HEADER header;

		::memset(&header, 0, sizeof(header));

		header.dwMagic = DDS_HEADER::MAGIC;
		header.dwSize = DDS_HEADER::SIZE;
		header.dwFlags = DDS_HEADER_FLAGS_TEXTURE;
		header.dwHeight = 16;
		header.dwWidth = 16;
		header.ddspf.dwSize = DDS_PIXELFORMAT::SIZE;
		header.ddspf.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
		header.ddspf.dwRGBBitCount = 32;
		header.ddspf.dwRBitMask = 0x00FF0000;
		header.ddspf.dwGBitMask = 0x0000FF00;
		header.ddspf.dwBBitMask = 0x000000FF;
		header.ddspf.dwABitMask = 0xFF000000;
		header.dwCaps = DDSCAPS_TEXTURE;

		if (file.write(header))
			write_color_gradient(file, true);
	}
	{
		File file("tests/image/intensity8.tga", File::Write | File::Truncate);

		TgaHeader header;

		::memset(&header, 0, sizeof(header));

		header.image_type = tgaBlackAndWhite;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 8;
		header.image.descriptor = tgaTopLeft;

		if (file.write(header))
			write_grayscale_intensity(file);
	}
	{
		File file("tests/image/intensity32.tga", File::Write | File::Truncate);

		TgaHeader header;

		::memset(&header, 0, sizeof(header));

		header.image_type = tgaTrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 32;
		header.image.descriptor = tgaTopLeft | 8;

		if (file.write(header))
			write_color_intensity(file);
	}
}
