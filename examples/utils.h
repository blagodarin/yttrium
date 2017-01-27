#ifndef _examples_utils_h_
#define _examples_utils_h_

#include <array>
#include <cstdio>
#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace
{
	std::string make_screenshot_path()
	{
		const auto date_time = boost::posix_time::second_clock::local_time();
		const auto date = date_time.date();
		const auto time = date_time.time_of_day();
		std::array<char, 24> buffer;
		std::snprintf(buffer.data(), buffer.size(), "%04d-%02d-%02d_%02d-%02d-%02d.png",
			static_cast<int>(date.year()), static_cast<int>(date.month()), static_cast<int>(date.day()), time.hours(), time.minutes(), time.seconds());
		return buffer.data();
	}
}

#endif
