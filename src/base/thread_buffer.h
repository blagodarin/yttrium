#ifndef __BASE_THREAD_BUFFER_H
#define __BASE_THREAD_BUFFER_H

#include <Yttrium/thread_buffer.h>

#include "condition.h"
#include "mutex.h"

namespace Yttrium
{

class Y_PRIVATE ThreadBufferBase::Private
{
public:

	Allocator       *allocator;
	StaticMutex      mutex;
	StaticCondition  read;
	StaticCondition  write;

	Private(Allocator *allocator)
		: allocator(allocator)
	{
	}
};

} // namespace Yttrium

#endif // __BASE_THREAD_BUFFER_H
