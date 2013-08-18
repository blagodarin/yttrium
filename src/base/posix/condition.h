#ifndef __BASE_POSIX_CONDITION_H
#define __BASE_POSIX_CONDITION_H

#include <yttrium/condition.h>

#include "../private_base.h"

#include <pthread.h>

namespace Yttrium
{

class Y_PRIVATE Condition::Private
	: public PrivateBase<Condition::Private>
{
public:

	pthread_cond_t cond;

public:

	Private(Allocator *allocator = nullptr);

	~Private();
};

class StaticCondition
	: public Condition
{
	Y_NONCOPYABLE(StaticCondition);

public:

	StaticCondition()
		: Condition(&_private_data)
	{
	}

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_SIGNAL_H
