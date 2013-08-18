#include <yttrium/time.h>

#include <yttrium/assert.h>

#include <time.h>

namespace Yttrium
{

DateTime DateTime::now()
{
	::timespec time;

	if (Y_UNLIKELY(::clock_gettime(CLOCK_REALTIME, &time)))
	{
		Y_ABORT("Failed to query CLOCK_REALTIME time");
	}
	else
	{
		::tm local_time;

		if (Y_UNLIKELY(!::localtime_r(&time.tv_sec, &local_time)))
		{
			Y_ABORT("Failed to decompose CLOCK_REALTIME time");
		}
		else
		{
			DateTime result;

			result.year    = local_time.tm_year + 1900;
			result.month   = local_time.tm_mon + 1;
			result.day     = local_time.tm_mday;
			result.hour    = local_time.tm_hour;
			result.minute  = local_time.tm_min;
			result.second  = local_time.tm_sec;
			result.msecond = time.tv_nsec / (1000 * 1000);

			return result;
		}
	}
}

Clock Timer::clock()
{
	::timespec time;

	if (Y_UNLIKELY(::clock_gettime(CLOCK_MONOTONIC, &time)))
	{
		Y_ABORT("Failed to query CLOCK_MONOTONIC time");
	}
	else
	{
		return time.tv_nsec / (1000 * 1000) + time.tv_sec * 1000;
	}
}

} // namespace Yttrium
