#ifndef __BASE_POSIX_SIGNAL_HPP
#define __BASE_POSIX_SIGNAL_HPP

#include <pthread.h>

#include <Yttrium/signal.hpp>

#include "../private_base.hpp"

namespace Yttrium
{

class Signal::Private: public PrivateBase<Signal::Private>
{
public:

	pthread_cond_t cond;

public:

	Private(Allocator *allocator = nullptr);
};

class StaticSignal: public Signal
{
public:

	StaticSignal()
		: Signal(&_private_data)
	{
	}

	StaticSignal(const StaticSignal &) = delete;

	StaticSignal &operator =(const StaticSignal &) = delete;

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_SIGNAL_HPP
