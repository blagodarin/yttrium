#ifndef __BASE_POSIX_MUTEX_H
#define __BASE_POSIX_MUTEX_H

#include <Yttrium/mutex.h>
#include <Yttrium/noncopyable.h>

#include "../private_base.h"

#include <pthread.h>

namespace Yttrium
{

class Y_PRIVATE Mutex::Private: public PrivateBase<Mutex::Private>
{
public:

	pthread_mutex_t mutex;

public:

	Private(Allocator *allocator = nullptr);
};

class StaticMutex: public Mutex, public Noncopyable
{
public:

	StaticMutex()
		: Mutex(&_private_data)
	{
	}

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_MUTEX_H
