#ifndef _examples_utils_h_
#define _examples_utils_h_

#include <yttrium/image.h>

#include <array>
#include <cstdio>
#include <string>
#include <tuple>

#include <boost/date_time/posix_time/posix_time_types.hpp>

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
	const auto date_time = boost::posix_time::second_clock::local_time();
	const auto date = date_time.date();
	const auto time = date_time.time_of_day();
	std::array<char, 24> buffer;
	std::snprintf(buffer.data(), buffer.size(), "%04d-%02d-%02d_%02d-%02d-%02d.png",
		static_cast<int>(date.year()), static_cast<int>(date.month()), static_cast<int>(date.day()), time.hours(), time.minutes(), time.seconds());
	return buffer.data();
}

#endif
