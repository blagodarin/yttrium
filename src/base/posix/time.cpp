#include <yttrium/time.h>

#include <ctime>
#include <system_error>

namespace Yttrium
{
	DateTime DateTime::now()
	{
		::timespec time;
		if (0 != ::clock_gettime(CLOCK_REALTIME, &time))
			throw std::system_error(errno, std::generic_category());

		::tm local_time;
		if (&local_time != ::localtime_r(&time.tv_sec, &local_time))
			throw std::system_error(EINVAL, std::generic_category());

		DateTime result;
		result.year = local_time.tm_year + 1900;
		result.month = local_time.tm_mon + 1;
		result.day = local_time.tm_mday;
		result.hour = local_time.tm_hour;
		result.minute = local_time.tm_min;
		result.second = local_time.tm_sec;
		result.msecond = time.tv_nsec / (1000 * 1000);
		return result;
	}
}
