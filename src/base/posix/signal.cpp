#include "signal.h"

#include <Yttrium/assert.h>

#include "mutex.h"

#include <time.h> // clock_gettime, timespec

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

Signal::Private::~Private()
{
	if (pthread_cond_destroy(&cond))
	{
		Y_ABORT("Can't destroy signal");
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

} // namespace Yttrium
