#ifndef __BASE_POSIX_THREAD_HPP
#define __BASE_POSIX_THREAD_HPP

#include <atomic>

#include <pthread.h>

#include <Yttrium/thread.hpp>

#include "../private_base.hpp"

namespace Yttrium
{

class Thread::Private: public PrivateBase<Thread::Private>
{
public:

	std::atomic<bool> _is_running;
	Thread::Function  _function;
	pthread_t         _handle;

public:

	Private(Allocator *allocator)
		: PrivateBase(allocator)
		, _is_running(false)
	{
	}

public:

	static void *entry_point(void *data);
};

} // namespace Yttrium

#endif // __BASE_POSIX_THREAD_HPP
