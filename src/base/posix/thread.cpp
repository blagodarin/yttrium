#include "thread.hpp"

#include <sched.h> // sched_yield
#include <time.h>  // nanosleep, timespec

#include <Yttrium/assert.hpp>

namespace Yttrium
{

bool Thread::is_running()
{
	return _private->_is_running;
}

void Thread::start(const Function &function)
{
	if (_private->_is_running)
	{
		Y_ABORT("The thread has already been started");
		return;
	}

	pthread_t handle;

	_private->_function = function;
	if (pthread_create(&handle, NULL, &Private::entry_point, _private))
	{
		Y_ABORT("Can't start a thread");
		return;
	}

	_private->_is_running = true;
	_private->_handle = handle;
}

void Thread::stop()
{
	if (_private->_is_running)
	{
		if (pthread_cancel(_private->_handle))
		{
			Y_ABORT("Can't stop a thread");
		}
		_private->_is_running = false;
	}
}

void Thread::wait()
{
	if (_private->_is_running && !pthread_equal(_private->_handle, pthread_self()))
	{
		void *result;

		if (pthread_join(_private->_handle, &result))
		{
			Y_ABORT("Can't wait for a thread");
		}
	}
}

void *Thread::Private::entry_point(void *data)
{
	Private *thread_private = static_cast<Private *>(data);

	thread_private->_function();
	thread_private->_is_running = false;
	return NULL;
}

void Thread::sleep(Clock milliseconds)
{
	Y_ASSERT(milliseconds >= 0);

	if (milliseconds)
	{
		struct timespec time;

		time.tv_sec = milliseconds / 1000;
		time.tv_nsec = (milliseconds % 1000) * 1000 * 1000;

		if (nanosleep(&time, nullptr))
		{
			Y_ABORT("Can't sleep");
		}
	}
	else
	{
	#ifdef _POSIX_PRIORITY_SCHEDULING // Defined for 'sched_yield'.
		if (sched_yield())
	#else
		if (pthread_yield())
	#endif
		{
			Y_ABORT("Can't sleep for 0 ms");
		}
	}
}

void Thread::close()
{
	if (Private::should_free(&_private))
	{
		if (_private->_is_running)
		{
			Y_ABORT("The thread must be terminated explicitly");
		}
		Private::free(&_private);
	}
}

} // namespace Yttrium
