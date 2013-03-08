#include "condition.h"

#include <Yttrium/assert.h>

#include "mutex.h"

#include <errno.h> // ETIMEDOUT
#include <time.h>  // clock_gettime, timespec

namespace Yttrium
{

Condition::Private::Private(Allocator *allocator)
	: PrivateBase(allocator)
{
	if (Y_UNLIKELY(::pthread_cond_init(&cond, nullptr)))
	{
		Y_ABORT("Can't create condition");
	}
}

Condition::Private::~Private()
{
	if (Y_UNLIKELY(::pthread_cond_destroy(&cond)))
	{
		Y_ABORT("Can't destroy condition"); // NOTE: Safe to continue (Y_ASSERT?).
	}
}

void Condition::notify_all()
{
	if (Y_UNLIKELY(::pthread_cond_broadcast(&_private->cond)))
	{
		Y_ABORT("Condition::notify_all() failed");
	}
}

void Condition::notify_one()
{
	if (Y_UNLIKELY(::pthread_cond_signal(&_private->cond)))
	{
		Y_ABORT("Condition::notify_one() failed");
	}
}

bool Condition::try_wait(Mutex *mutex)
{
	::timespec time;

	if (Y_UNLIKELY(::clock_gettime(CLOCK_REALTIME, &time)))
	{
		Y_ABORT("clock_gettime(CLOCK_REALTIME, ...) failed"); // NOTE: Safe to continue (Y_ASSERT?).
		return false;
	}

	int result = ::pthread_cond_timedwait(&_private->cond, &mutex->_private->mutex, &time);

	Y_ABORT_IF(result && result != ETIMEDOUT, "Can't try-wait for condition"); // NOTE: Safe to continue.

	return !result;
}

bool Condition::try_wait(Mutex *mutex, Clock milliseconds)
{
	::timespec time;

	if (Y_UNLIKELY(::clock_gettime(CLOCK_REALTIME, &time)))
	{
		Y_ABORT("clock_gettime(CLOCK_REALTIME, ...) failed"); // NOTE: Safe to continue (Y_ASSERT?).
		return false;
	}

	time.tv_sec += milliseconds / 1000;
	time.tv_nsec += (milliseconds % 1000) * 1000 * 1000;
	if (time.tv_nsec >= 1000000000)
	{
		time.tv_nsec -= 1000000000;
		time.tv_sec  += 1;
	}

	int result = ::pthread_cond_timedwait(&_private->cond, &mutex->_private->mutex, &time);

	Y_ABORT_IF(result && result != ETIMEDOUT, "Can't timed-wait for condition"); // NOTE: Safe to continue.

	return !result;
}

void Condition::wait(Mutex *mutex)
{
	if (Y_UNLIKELY(::pthread_cond_wait(&_private->cond, &mutex->_private->mutex)))
	{
		Y_ABORT("Can't wait for condition");
	}
}

} // namespace Yttrium
