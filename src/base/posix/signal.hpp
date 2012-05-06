#ifndef __BASE_POSIX_SIGNAL_HPP
#define __BASE_POSIX_SIGNAL_HPP

#include <pthread.h>

#include <Yttrium/noncopyable.hpp>
#include <Yttrium/signal.hpp>

#include "../private_base.hpp"

namespace Yttrium
{

class Y_PRIVATE Signal::Private: public PrivateBase<Signal::Private>
{
public:

	pthread_cond_t cond;

public:

	Private(Allocator *allocator = nullptr);
};

class StaticSignal: public Signal, public Noncopyable
{
public:

	StaticSignal()
		: Signal(&_private_data)
	{
	}

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_SIGNAL_HPP
