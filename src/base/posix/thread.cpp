#include "thread.hpp"

#include <sched.h>  // sched_yield
#include <unistd.h> // usleep

#include <Yttrium/assert.hpp>

namespace Yttrium
{

Thread::Thread(Allocator *allocator)
	: _private(new(allocator->allocate<Private>())
		Private(allocator))
{
}

Thread::Thread(const Thread &thread)
	: _private(Private::copy(thread._private))
{
}

Thread::~Thread()
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

bool Thread::is_running()
{
	return _private->_is_running;
}

void Thread::start(const Function &function)
{
	if (_private->_is_running)
	{
		Y_ABORT("The thread has already been started");
	}

	pthread_t handle;

	_private->_function = function;
	if (pthread_create(&handle, NULL, &Private::entry_point, _private))
	{
		Y_ABORT("Can't start a thread");
	}

	_private->_is_running = true;
	_private->_handle = handle;
}

void Thread::stop()
{
	if (_private->_is_running)
	{
		pthread_cancel(_private->_handle);
		_private->_is_running = false;
	}
}

void Thread::wait()
{
	if (_private->_is_running && !pthread_equal(_private->_handle, pthread_self()))
	{
		void *result;

		pthread_join(_private->_handle, &result);
	}
}

void *Thread::Private::entry_point(void *data)
{
	Private *thread_private = static_cast<Private *>(data);

	thread_private->_function();
	thread_private->_is_running = false;
	return NULL;
}

void Thread::sleep(Clock ms)
{
	if (ms)
	{
		usleep(ms * 1000); // NOTE: May overflow.
	}
	else
	{
	#ifdef _POSIX_PRIORITY_SCHEDULING // Defined for 'sched_yield'.
		sched_yield();
	#else
		pthread_yield();
	#endif
	}
}

} // namespace Yttrium
