#include <yttrium/date_time.h>

#include <yttrium/assert.h>

#include <time.h>

namespace Yttrium
{
	DateTime DateTime::now()
	{
		::timespec time;
		Y_VERIFY(0, ::clock_gettime(CLOCK_REALTIME, &time));

		::tm local_time;
		Y_VERIFY(&local_time, ::localtime_r(&time.tv_sec, &local_time));

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
