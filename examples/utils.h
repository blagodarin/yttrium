#ifndef _examples_utils_h_
#define _examples_utils_h_

#include <yttrium/image.h>
#include <yttrium/math/color.h>

#include <array>
#include <ctime>

inline auto make_bgra_tga(size_t width, size_t height, const std::function<Yttrium::Bgra32(size_t, size_t)>& callback)
{
	Yttrium::Image image({ width, height, Yttrium::PixelFormat::Bgra32 });
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			const auto pixel = static_cast<uint8_t*>(image.data()) + y * image.format().row_size() + x * 4;
			const auto color = callback(x, y);
			pixel[0] = color._b;
			pixel[1] = color._g;
			pixel[2] = color._r;
			pixel[3] = color._a;
		}
	}
	return image.to_buffer(Yttrium::ImageType::Tga);
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

#endif
