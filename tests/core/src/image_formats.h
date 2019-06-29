//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#pragma once

#include <yttrium/image.h>

template <unsigned size>
inline Yttrium::Image make_gray8_test_image()
{
	Yttrium::Buffer buffer{ size * size };
	auto data = &buffer[0];
	for (unsigned y = 0; y < size; ++y)
		for (unsigned x = 0; x < size; ++x)
			*data++ = static_cast<std::uint8_t>(y < size / 2 ? x * size + y / 2 : (size - 1 - x) * size + (size - 1 - y) / 2);
	return { { size, size, Yttrium::PixelFormat::Gray8 }, buffer.data() };
}

inline Yttrium::Image make_test_image(bool with_alpha, Yttrium::ImageOrientation orientation = Yttrium::ImageOrientation::XRightYDown)
{
	Yttrium::Buffer buffer(16 * 16 * (with_alpha ? 4 : 3));
	auto data = &buffer[0];
	for (int row = 0; row < 16; ++row)
	{
		const auto y = orientation == Yttrium::ImageOrientation::XRightYDown ? row : 15 - row;
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
	return { { 16, 16, with_alpha ? Yttrium::PixelFormat::Bgra32 : Yttrium::PixelFormat::Bgr24, orientation }, buffer.data() };
}
