#include <Yttrium/time.h>

#include <time.h> // clock_gettime, time, time_t, tm, localtime_r

#include <Yttrium/assert.h>

namespace Yttrium
{

DateTime DateTime::now()
{
	time_t    unix_time;
	struct tm local_time;
	DateTime  result;

	if (time(&unix_time) == -1 || !localtime_r(&unix_time, &local_time))
	{
		Y_ABORT("Can't get the current date and time");
	}
	else
	{
		result.year   = local_time.tm_year + 1900;
		result.month  = local_time.tm_mon + 1;
		result.day    = local_time.tm_mday;
		result.hour   = local_time.tm_hour;
		result.minute = local_time.tm_min;
		result.second = local_time.tm_sec;
	}

	return result;
}

Clock Timer::clock()
{
	struct timespec time;

	if (clock_gettime(CLOCK_MONOTONIC, &time))
	{
		Y_ABORT("clock_gettime(CLOCK_MONOTONIC, ...) failed");
		return 0;
	}

	return time.tv_nsec / (1000 * 1000) + time.tv_sec * 1000;
}

} // namespace Yttrium
