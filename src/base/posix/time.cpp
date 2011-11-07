#include <Yttrium/time.hpp>

#include <time.h>

namespace Yttrium
{

DateTime DateTime::now() throw()
{
	time_t    unix_time;
	struct tm local_time;
	DateTime  result;

	time(&unix_time);
	localtime_r(&unix_time, &local_time);
	result.year = local_time.tm_year + 1900;
	result.month = local_time.tm_mon + 1;
	result.day = local_time.tm_mday;
	result.hour = local_time.tm_hour;
	result.minute = local_time.tm_min;
	result.second = local_time.tm_sec;
	return result;
}

Clock Timer::clock() throw()
{
	struct timespec time;

	clock_gettime(CLOCK_MONOTONIC, &time);
	return time.tv_nsec / (1000 * 1000) + time.tv_sec * 1000;
}

} // namespace Yttrium
