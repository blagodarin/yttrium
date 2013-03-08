#ifndef __BASE_POSIX_MUTEX_H
#define __BASE_POSIX_MUTEX_H

#include <Yttrium/mutex.h>

#include "../private_base.h"

#include <pthread.h>

namespace Yttrium
{

class Y_PRIVATE Mutex::Private
	: public PrivateBase<Mutex::Private>
{
public:

	pthread_mutex_t mutex;

public:

	Private(Allocator *allocator = nullptr);

	~Private();
};

class StaticMutex
	: public Mutex
{
	Y_NONCOPYABLE(StaticMutex);

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
