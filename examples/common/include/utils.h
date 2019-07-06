//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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
#include <yttrium/math/color.h>

#include <array>
#include <ctime>
#include <functional>

inline auto make_bgra_tga(size_t width, size_t height, const std::function<Yttrium::Bgra32(size_t, size_t)>& callback)
{
	Yttrium::Image image({ width, height, Yttrium::PixelFormat::Bgra32 });
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			const auto pixel = static_cast<uint8_t*>(image.data()) + y * image.info().stride() + x * 4;
			const auto color = callback(x, y);
			pixel[0] = color._b;
			pixel[1] = color._g;
			pixel[2] = color._r;
			pixel[3] = color._a;
		}
	}
	return image.to_buffer(Yttrium::ImageFormat::Tga);
}

inline std::string make_screenshot_path()
{
	const auto time = std::time(nullptr);
	::tm tm;
#ifdef _MSC_VER
	::localtime_s(&tm, &time);
#else
	::localtime_r(&time, &tm);
#endif
	std::array<char, 24> buffer;
	buffer[std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d_%H-%M-%S.png", &tm)] = '\0';
	return buffer.data();
}
