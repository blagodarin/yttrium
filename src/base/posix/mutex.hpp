#ifndef __BASE_POSIX_MUTEX_HPP
#define __BASE_POSIX_MUTEX_HPP

#include <pthread.h>

#include <Yttrium/mutex.hpp>

#include "../private_base.hpp"

namespace Yttrium
{

class Mutex::Private: public PrivateBase<Mutex::Private>
{
public:

	pthread_mutex_t mutex;

public:

	Private(Allocator *allocator = nullptr);
};

class StaticMutex: public Mutex
{
public:

	StaticMutex()
		: Mutex(&_private_data)
	{
	}

	StaticMutex(const StaticMutex &) = delete;

	StaticMutex &operator =(const StaticMutex &) = delete;

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_MUTEX_HPP
