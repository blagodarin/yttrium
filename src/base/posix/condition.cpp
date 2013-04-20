#include "condition.h"

#include <Yttrium/assert.h>

#include "mutex.h"
#include "time.h"

#include <errno.h> // ETIMEDOUT

namespace Yttrium
{

Condition::Private::Private(Allocator *allocator)
	: PrivateBase(allocator)
{
	if (Y_UNLIKELY(::pthread_cond_init(&cond, nullptr)))
	{
		Y_ABORT("Failed to create a condition");
	}
}

Condition::Private::~Private()
{
	if (Y_UNLIKELY(::pthread_cond_destroy(&cond)))
	{
		Y_ABORT("Failed to destroy a condition");
	}
}

void Condition::notify_all()
{
	if (Y_UNLIKELY(::pthread_cond_broadcast(&_private->cond)))
	{
		Y_ABORT("Failed to notify condition subscribers");
	}
}

void Condition::notify_one()
{
	if (Y_UNLIKELY(::pthread_cond_signal(&_private->cond)))
	{
		Y_ABORT("Failed to notify a condition subscriber");
	}
}

bool Condition::try_wait(Mutex *mutex)
{
	::timespec time;

	if (Y_UNLIKELY(::clock_gettime(CLOCK_REALTIME, &time)))
	{
		Y_ABORT("Failed to query CLOCK_REALTIME time");
	}
	else
	{
		const int result = ::pthread_cond_timedwait(&_private->cond, &mutex->_private->mutex, &time);
		
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
			Y_ABORT("Failed to try-wait for a condition");
		}
	}
}

bool Condition::try_wait(Mutex *mutex, Clock milliseconds)
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

		const int result = ::pthread_cond_timedwait(&_private->cond, &mutex->_private->mutex, &time);

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
			Y_ABORT("Failed to timed-wait for a condition");
		}
	}
}

void Condition::wait(Mutex *mutex)
{
	if (Y_UNLIKELY(::pthread_cond_wait(&_private->cond, &mutex->_private->mutex)))
	{
		Y_ABORT("Failed to wait for a condition");
	}
}

} // namespace Yttrium
