// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/image/image.h>

template <unsigned size>
inline Yt::Image make_gray8_test_image()
{
	Yt::Buffer buffer{ size * size };
	auto data = &buffer[0];
	for (unsigned y = 0; y < size; ++y)
		for (unsigned x = 0; x < size; ++x)
			*data++ = static_cast<uint8_t>(y < size / 2 ? x * size + y / 2 : (size - 1 - x) * size + (size - 1 - y) / 2);
	return { { size, size, Yt::PixelFormat::Gray8 }, buffer.data() };
}

inline Yt::Image make_test_image(bool with_alpha, Yt::ImageOrientation orientation = Yt::ImageOrientation::XRightYDown)
{
	Yt::Buffer buffer(16 * 16 * (with_alpha ? 4 : 3));
	auto data = &buffer[0];
	for (int row = 0; row < 16; ++row)
	{
		const auto y = orientation == Yt::ImageOrientation::XRightYDown ? row : 15 - row;
		for (int x = 0; x < 16; ++x)
		{
			int b = 0;
			int g = 0;
			int r = 0;
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
				b = ((y - 8) * 16 + x) * 4 + 3;
			}
			else
			{
				b = ((15 - y) * 16 + x) * 4 + 3;
			}
			*data++ = static_cast<uint8_t>(b);
			*data++ = static_cast<uint8_t>(g);
			*data++ = static_cast<uint8_t>(r);
			if (with_alpha)
				*data++ = static_cast<uint8_t>(x * 16 + 15);
		}
	}
	return { { 16, 16, with_alpha ? Yt::PixelFormat::Bgra32 : Yt::PixelFormat::Bgr24, orientation }, buffer.data() };
}
