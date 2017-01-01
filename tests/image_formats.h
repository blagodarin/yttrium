#ifndef _tests_image_formats_h_
#define _tests_image_formats_h_

#include <yttrium/image.h>

inline Yttrium::Image make_test_image(bool with_alpha)
{
	Yttrium::Buffer buffer(16 * 16 * (with_alpha ? 4 : 3));
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
	return { { 16, 16, with_alpha ? Yttrium::PixelFormat::Bgra : Yttrium::PixelFormat::Bgr, with_alpha ? 32u : 24u }, buffer.data() };
}

#endif
