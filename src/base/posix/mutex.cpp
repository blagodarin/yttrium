#include "mutex.hpp"

#include <time.h>

#include <Yttrium/assert.hpp>

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

Mutex::Mutex(Allocator *allocator)
	: _private(new(allocator->allocate<Private>())
		Private(allocator))
{
}

Mutex::~Mutex()
{
	if (Private::should_free(&_private))
	{
		if (pthread_mutex_destroy(&_private->mutex))
		{
			Y_ABORT("Can't destroy mutex");
		}
		Private::free(&_private);
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
	return (result == 0);
}

bool Mutex::try_lock(Clock milliseconds)
{
	struct timespec time;

	clock_gettime(CLOCK_REALTIME, &time);
	time.tv_sec += milliseconds / 1000;
	time.tv_nsec += (milliseconds % 1000) * 1000 * 1000;

	int result = pthread_mutex_timedlock(&_private->mutex, &time);
	if (result && result != ETIMEDOUT)
	{
		Y_ABORT("Can't timed-lock mutex");
	}
	return (result == 0);
}

void Mutex::unlock()
{
	if (pthread_mutex_unlock(&_private->mutex))
	{
		Y_ABORT("Can't unlock mutex");
	}
}

} // namespace Yttrium
