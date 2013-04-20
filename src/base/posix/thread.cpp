#include "thread.h"

#include <Yttrium/assert.h>

#include <errno.h> // errno
#include <sched.h> // sched_yield
#include <time.h>  // nanosleep, timespec

namespace Yttrium
{

bool Thread::is_running()
{
	return _private->_is_running;
}

void Thread::start()
{
	Y_ASSERT(!is_running());

	if (Y_LIKELY(!_private->_is_running))
	{
		_private->_is_running = true;
		if (Y_UNLIKELY(::pthread_create(&_private->_handle, nullptr, &Private::entry_point, this)))
		{
			_private->_is_running = false;
			Y_ABORT("Failed to start a thread");
		}
	}
}

void Thread::stop()
{
	if (_private->_is_running)
	{
		if (Y_UNLIKELY(::pthread_cancel(_private->_handle)))
		{
			Y_ABORT("Failed to stop a thread");
		}
		else
		{
			void *result;

			if (Y_UNLIKELY(::pthread_join(_private->_handle, &result)))
			{
				Y_ABORT("Failed to wait for a stopped thread");
			}
		}
		_private->_is_running = false;
	}
}

void Thread::wait()
{
	if (_private->_is_running && Y_LIKELY(!::pthread_equal(_private->_handle, ::pthread_self())))
	{
		void *result;

		if (Y_UNLIKELY(::pthread_join(_private->_handle, &result)))
		{
			Y_ABORT("Failed to wait for a thread");
		}
	}
}

void *Thread::Private::entry_point(void *data)
{
	Thread *thread = static_cast<Thread *>(data);

	thread->run();
	thread->_private->_is_running = false;

	return nullptr;
}

void Thread::sleep(Clock milliseconds)
{
	Y_ASSERT(milliseconds >= 0);

	if (Y_LIKELY(milliseconds > 0))
	{
		::timespec time;

		time.tv_sec = milliseconds / 1000;
		time.tv_nsec = (milliseconds % 1000) * 1000 * 1000;

		if (Y_UNLIKELY(::nanosleep(&time, nullptr)))
		{
			Y_ABORT("Failed to sleep");
		}
	}
	else
	{
	#if defined(_POSIX_PRIORITY_SCHEDULING)
		if (Y_UNLIKELY(::sched_yield()))
	#else
		if (Y_UNLIKELY(::pthread_yield()))
	#endif
		{
			Y_ABORT("Failed to yield");
		}
	}
}

} // namespace Yttrium
