#include "mutex.h"

#include <Yttrium/assert.h>

#include <errno.h> // EBUSY, ETIMEDOUT
#include <time.h>  // clock_gettime, timespec

namespace Yttrium
{

Mutex::Private::Private(Allocator *allocator)
	: PrivateBase(allocator)
{
	if (pthread_mutex_init(&mutex, nullptr))
	{
		// Either the system is out of resources (EAGAIN or ENOMEM), or there
		// are no privileges to create a mutex (EPERM). There's nothing we can
		// do in both cases.

		Y_ABORT("Can't create mutex");
	}
}

Mutex::Private::~Private()
{
	if (pthread_mutex_destroy(&mutex))
	{
		// Either the mutex is invalid (EINVAL), or it is locked or referenced
		// (EBUSY). Since it can't be invalid (it should have been aborted in
		// the constructor), the only remaining option is...

		Y_ABORT("Can't destroy a busy mutex");
	}
}

void Mutex::lock()
{
	if (pthread_mutex_lock(&_private->mutex))
	{
		// This should never happen with our mutexes.

		Y_ABORT("Can't lock mutex");
	}
}

bool Mutex::try_lock()
{
	int result = pthread_mutex_trylock(&_private->mutex);

	Y_ABORT_IF(result && result != EBUSY, "Can't try-lock mutex"); // NOTE: Safe to continue.

	return !result;
}

bool Mutex::try_lock(Clock milliseconds)
{
	struct timespec time;

	if (clock_gettime(CLOCK_REALTIME, &time))
	{
		// This must be EINVAL, telling us that there's no CLOCK_REALTIME clock
		// on this system (is it actually possible?).

		Y_ABORT("Can't query CLOCK_REALTIME time");
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

	Y_ABORT_IF(result && result != ETIMEDOUT, "Can't timed-lock mutex"); // NOTE: Safe to continue.

	return !result;
}

void Mutex::unlock()
{
	if (pthread_mutex_unlock(&_private->mutex))
	{
		// This must be EPERM, indicating that we don't own the mutex.

		Y_ABORT("Can't unlock mutex");
	}
}

} // namespace Yttrium
