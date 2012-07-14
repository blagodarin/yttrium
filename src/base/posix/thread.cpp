#include "thread.h"

#include <Yttrium/assert.h>

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
	if (_private->_is_running)
	{
		Y_ABORT("The thread has already been started");
		return;
	}

	_private->_is_running = true;

	if (pthread_create(&_private->_handle, nullptr, &Private::entry_point, this))
	{
		Y_ABORT("Can't start a thread");
		_private->_is_running = false;
		return;
	}
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
	Thread *thread = static_cast<Thread *>(data);

	thread->run();

	thread->_private->_is_running = false;

	return nullptr;
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

} // namespace Yttrium
