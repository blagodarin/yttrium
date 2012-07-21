#include "mutex.h"

#include <Yttrium/assert.h>

#include <time.h> // clock_gettime, timespec

namespace Yttrium
{

Mutex::Private::Private(Allocator *allocator)
	: PrivateBase(allocator)
{
	if (pthread_mutex_init(&mutex, nullptr))
	{
		Y_ABORT("Can't create mutex");
	}
}

Mutex::Private::~Private()
{
	if (pthread_mutex_destroy(&mutex))
	{
		Y_ABORT("Can't destroy mutex");
	}
}

void Mutex::lock()
{
	if (pthread_mutex_lock(&_private->mutex))
	{
		Y_ABORT("Can't lock mutex");
	}
}

bool Mutex::try_lock()
{
	int result = pthread_mutex_trylock(&_private->mutex);
	if (result && result != EBUSY)
	{
		Y_ABORT("Can't try-lock mutex");
	}
	return !result;
}

bool Mutex::try_lock(Clock milliseconds)
{
	struct timespec time;

	if (clock_gettime(CLOCK_REALTIME, &time))
	{
		Y_ABORT("clock_gettime(CLOCK_REALTIME, ...) failed");
		return false;
	}

	time.tv_sec += milliseconds / 1000;
	time.tv_nsec += (milliseconds % 1000) * 1000 * 1000;
	if (time.tv_nsec >= 1000000000)
	{
		time.tv_nsec -= 1000000000;
		time.tv_sec  += 1;
	}

	int result = pthread_mutex_timedlock(&_private->mutex, &time);
	if (result && result != ETIMEDOUT)
	{
		Y_ABORT("Can't timed-lock mutex");
	}
	return !result;
}

void Mutex::unlock()
{
	if (pthread_mutex_unlock(&_private->mutex))
	{
		Y_ABORT("Can't unlock mutex");
	}
}

} // namespace Yttrium
