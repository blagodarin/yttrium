//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/storage/writer.h>
#include "../../libs/core/image/formats/dds.h"
#include "../../libs/core/image/formats/tga.h"
#include "../core/image_formats.h"

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
