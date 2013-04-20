#include "mutex.h"

#include <Yttrium/assert.h>

#include "time.h"

#include <errno.h> // EBUSY, ETIMEDOUT

namespace Yttrium
{

Mutex::Private::Private(Allocator *allocator)
	: PrivateBase(allocator)
{
	if (Y_UNLIKELY(::pthread_mutex_init(&mutex, nullptr)))
	{
		// Either the system is out of resources (EAGAIN or ENOMEM), or there
		// are no privileges to create a mutex (EPERM). There's nothing we can
		// do in both cases.

		Y_ABORT("Failed to create a mutex");
	}
}

Mutex::Private::~Private()
{
	if (Y_UNLIKELY(::pthread_mutex_destroy(&mutex)))
	{
		// Either the mutex is invalid (EINVAL), or it is locked or referenced
		// (EBUSY). Since it can't be invalid (it must have been aborted in
		// the constructor), the only remaining option is the latter one.

		Y_ABORT("Failed to destroy a busy mutex");
	}
}

void Mutex::lock()
{
	if (Y_UNLIKELY(::pthread_mutex_lock(&_private->mutex)))
	{
		Y_ABORT("Failed to lock a mutex");
	}
}

bool Mutex::try_lock()
{
	const int result = ::pthread_mutex_trylock(&_private->mutex);

	if (!result)
	{
		return true;
	}
	else if (Y_LIKELY(result == EBUSY))
	{
		return false;
	}
	else
	{
		Y_ABORT("Failed to try-lock a mutex");
	}
}

bool Mutex::try_lock(Clock milliseconds)
{
	Y_ASSERT(milliseconds > 0);

	::timespec time;

	if (Y_UNLIKELY(::clock_gettime(CLOCK_REALTIME, &time)))
	{
		Y_ABORT("Failed to query CLOCK_REALTIME time");
	}
	else
	{
		add_mseconds(&time, milliseconds);

		const int result = ::pthread_mutex_timedlock(&_private->mutex, &time);

		if (!result)
		{
			return true;
		}
		else if (Y_LIKELY(result == ETIMEDOUT))
		{
			return false;
		}
		else
		{
			Y_ABORT("Failed to timed-lock a mutex");
		}
	}
}

void Mutex::unlock()
{
	if (Y_UNLIKELY(::pthread_mutex_unlock(&_private->mutex)))
	{
		// This must be EPERM, indicating that we don't own the mutex.

		Y_ABORT("Failed to unlock a mutex");
	}
}

} // namespace Yttrium
