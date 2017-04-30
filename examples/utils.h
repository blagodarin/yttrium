#ifndef _examples_utils_h_
#define _examples_utils_h_

#include <yttrium/image.h>

#include <iomanip>
#include <sstream>
#include <tuple>

inline auto make_bgra_tga(size_t width, size_t height, const std::function<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>(size_t, size_t)>& callback)
{
	Yttrium::Image image({ width, height, Yttrium::PixelFormat::Bgra, 32 });
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			const auto pixel = static_cast<uint8_t*>(image.data()) + y * image.format().row_size() + x * 4;
			const auto color = callback(x, y);
			pixel[0] = std::get<0>(color);
			pixel[1] = std::get<1>(color);
			pixel[2] = std::get<2>(color);
			pixel[3] = std::get<3>(color);
		}
	}
	return image.to_buffer(Yttrium::ImageType::Tga);
}

inline std::string make_screenshot_path()
{
	const auto t = std::time(nullptr);
	std::ostringstream s;
	s << std::put_time(std::localtime(&t), "%Y-%m-%d_%H-%M-%S.png");
	return s.str();
}

#endif
