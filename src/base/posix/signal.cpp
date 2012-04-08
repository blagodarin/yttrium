#include "signal.hpp"

#include <time.h> // clock_gettime, timespec

#include <Yttrium/assert.hpp>

#include "mutex.hpp"

namespace Yttrium
{

Signal::Private::Private(Allocator *allocator)
	: PrivateBase(allocator)
{
	if (pthread_cond_init(&cond, nullptr))
	{
		Y_ABORT("Can't create signal");
	}
}

void Signal::wait(Mutex *mutex)
{
	if (pthread_cond_wait(&_private->cond, &mutex->_private->mutex))
	{
		Y_ABORT("Can't wait for signal");
	}
}

bool Signal::try_wait(Mutex *mutex)
{
	struct timespec time;

	if (clock_gettime(CLOCK_REALTIME, &time))
	{
		Y_ABORT("clock_gettime(CLOCK_REALTIME, ...) failed");
		return false;
	}

	int result = pthread_cond_timedwait(&_private->cond, &mutex->_private->mutex, &time);
	if (result && result != ETIMEDOUT)
	{
		Y_ABORT("Can't try-wait for signal");
	}
	return !result;
}

bool Signal::try_wait(Mutex *mutex, Clock milliseconds)
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

	int result = pthread_cond_timedwait(&_private->cond, &mutex->_private->mutex, &time);
	if (result && result != ETIMEDOUT)
	{
		Y_ABORT("Can't timed-wait for signal");
	}
	return !result;
}

void Signal::signal()
{
	if (pthread_cond_signal(&_private->cond))
	{
		Y_ABORT("Can't signal");
	}
}

void Signal::close()
{
	if (Private::should_free(&_private))
	{
		if (pthread_cond_destroy(&_private->cond))
		{
			Y_ABORT("Can't destroy signal");
		}
		Private::free(&_private);
	}
}

} // namespace Yttrium
