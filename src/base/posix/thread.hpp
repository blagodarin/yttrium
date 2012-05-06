#ifndef __BASE_POSIX_THREAD_HPP
#define __BASE_POSIX_THREAD_HPP

#include <atomic> // std::atomic

#include <pthread.h> // pthread_*

#include <Yttrium/thread.hpp>

#include "../private_base.hpp"

namespace Yttrium
{

class Y_PRIVATE Thread::Private
{
public:

	std::atomic<bool> _is_running;
	pthread_t         _handle;

public:

	Private()
		: _is_running(false)
	{
	}

public:

	static void *entry_point(void *data);
};

} // namespace Yttrium

#endif // __BASE_POSIX_THREAD_HPP
