#ifndef __BASE_POSIX_MUTEX_HPP
#define __BASE_POSIX_MUTEX_HPP

#include <pthread.h>

#include <Yttrium/mutex.hpp>
#include <Yttrium/noncopyable.hpp>

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

#endif // __BASE_POSIX_MUTEX_HPP
