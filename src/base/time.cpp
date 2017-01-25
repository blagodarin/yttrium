#include <yttrium/time.h>

#include <chrono>

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace Yttrium
{
	DateTime DateTime::now()
	{
		const auto date_time = boost::posix_time::microsec_clock::local_time();
		const auto date = date_time.date();
		const auto time = date_time.time_of_day();

		DateTime result;
		result.year = date.year();
		result.month = date.month();
		result.day = date.day();
		result.hour = time.hours();
		result.minute = time.minutes();
		result.second = time.seconds();
		result.msecond = time.total_milliseconds() % 1000;
		return result;
	}

	int millisecond_clock()
	{
		return std::chrono::high_resolution_clock::is_steady
			? std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()
			: std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}
}
