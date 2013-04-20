#ifndef __BASE_POSIX_TIME_H
#define __BASE_POSIX_TIME_H

#include <Yttrium/time.h>

#include <time.h>

namespace Yttrium
{

inline void add_mseconds(::timespec *time, Clock duration)
{
	time->tv_sec += duration / 1000;
	time->tv_nsec += (duration % 1000) * 1000 * 1000;
	if (time->tv_nsec >= 1000 * 1000 * 1000)
	{
		time->tv_nsec -= 1000 * 1000 * 1000;
		time->tv_sec  += 1;
	}
}

} // namespace Yttrium

#endif // __BASE_POSIX_TIME_H
