#ifndef __BASE_POSIX_THREAD_H
#define __BASE_POSIX_THREAD_H

#include <yttrium/thread.h>

#include "../private_base.h"

#include <atomic>
#include <pthread.h>

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

#endif // __BASE_POSIX_THREAD_H
