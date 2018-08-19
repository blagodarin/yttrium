#include <yttrium/storage/writer.h>
#include "../../libs/yttrium/image/formats/dds.h"
#include "../../libs/yttrium/image/formats/tga.h"
#include "../unit_tests/image_formats.h"

#include <cstring>

using namespace Yttrium;

namespace
{
	void write_color_gradient(Writer& writer, bool with_alpha)
	{
		const auto image = make_test_image(with_alpha);
		writer.write(image.data(), image.format().frame_size());
	}
}

int main()
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
}
