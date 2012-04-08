#include "signal.hpp"

#include <time.h>

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

Signal::Signal(Allocator *allocator)
	: _private(new(allocator->allocate<Private>())
		Private(allocator))
{
}

Signal::~Signal()
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

	clock_gettime(CLOCK_REALTIME, &time);

	int result = pthread_cond_timedwait(&_private->cond, &mutex->_private->mutex, &time);
	if (result && result != ETIMEDOUT)
	{
		Y_ABORT("Can't try-wait for signal");
	}
	return (result == 0);
}

bool Signal::try_wait(Mutex *mutex, Clock milliseconds)
{
	struct timespec time;

	clock_gettime(CLOCK_REALTIME, &time);
	time.tv_sec += milliseconds / 1000;
	time.tv_nsec += (milliseconds % 1000) * 1000 * 1000;

	int result = pthread_cond_timedwait(&_private->cond, &mutex->_private->mutex, &time);
	if (result && result != ETIMEDOUT)
	{
		Y_ABORT("Can't timed-wait for signal");
	}
	return (result == 0);
}

void Signal::signal()
{
	if (pthread_cond_signal(&_private->cond))
	{
		Y_ABORT("Can't signal");
	}
}

} // namespace Yttrium
