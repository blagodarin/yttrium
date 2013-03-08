#ifndef __BASE_POSIX_SIGNAL_H
#define __BASE_POSIX_SIGNAL_H

#include <Yttrium/signal.h>

#include "../private_base.h"

#include <pthread.h>

namespace Yttrium
{

class Y_PRIVATE Signal::Private
	: public PrivateBase<Signal::Private>
{
public:

	pthread_cond_t cond;

public:

	Private(Allocator *allocator = nullptr);

	~Private();
};

class StaticSignal
	: public Signal
{
	Y_NONCOPYABLE(StaticSignal);

public:

	StaticSignal()
		: Signal(&_private_data)
	{
	}

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_SIGNAL_H
