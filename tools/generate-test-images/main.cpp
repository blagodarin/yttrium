#include <yttrium/memory/buffer.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/writer.h>
#include "../../src/image/formats/dds.h"
#include "../../src/image/formats/tga.h"

#include <cstring>

using namespace Yttrium;

void write_color_gradient(Writer& writer, bool with_alpha)
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

	writer.write_all(buffer);
}

void write_y8(Writer& writer)
{
	Buffer buffer(16 * 16);

	auto data = &buffer[0];

	for (size_t y = 0; y < 16; ++y)
		for (size_t x = 0; x < 16; ++x)
			*data++ = y * 16 + x;

	writer.write_all(buffer);
}

void write_y8_bgra32(Writer& writer)
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

	writer.write_all(buffer);
}

void write_y8_bgrx32(Writer& writer)
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
			*data++ = 0xff;
		}
	}

	writer.write_all(buffer);
}

int main(int, char**)
{
	{
		TgaHeader header;
		::memset(&header, 0, sizeof(header));
		header.image_type = tgaTrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 24;
		header.image.descriptor = tgaTopLeft;

		Writer writer("tests/image/gradient24.tga");
		if (writer.write(header))
			write_color_gradient(writer, false);
	}
	{
		TgaHeader header;
		::memset(&header, 0, sizeof(header));
		header.image_type = tgaTrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 32;
		header.image.descriptor = tgaTopLeft | 8;

		Writer writer("tests/image/gradient32.tga");
		if (writer.write(header))
			write_color_gradient(writer, true);
	}
	{
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

		Writer writer("tests/image/gradient32.dds");
		if (writer.write(header))
			write_color_gradient(writer, true);
	}
	{
		TgaHeader header;
		::memset(&header, 0, sizeof(header));
		header.image_type = tgaBlackAndWhite;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 8;
		header.image.descriptor = tgaTopLeft;

		Writer writer("tests/image/y8.tga");
		if (writer.write(header))
			write_y8(writer);
	}
	{
		TgaHeader header;
		::memset(&header, 0, sizeof(header));
		header.image_type = tgaTrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 32;
		header.image.descriptor = tgaTopLeft | 8;

		Writer writer("tests/image/y8_bgra32.tga");
		if (writer.write(header))
			write_y8_bgra32(writer);
	}
	{
		TgaHeader header;
		::memset(&header, 0, sizeof(header));
		header.image_type = tgaTrueColor;
		header.image.width = 16;
		header.image.height = 16;
		header.image.pixel_depth = 32;
		header.image.descriptor = tgaTopLeft | 8;

		Writer writer("tests/image/y8_bgrx32.tga");
		if (writer.write(header))
			write_y8_bgrx32(writer);
	}
}
