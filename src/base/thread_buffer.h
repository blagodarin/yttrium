#ifndef __BASE_THREAD_BUFFER_H
#define __BASE_THREAD_BUFFER_H

#include <Yttrium/thread_buffer.h>

#include "mutex.h"
#include "signal.h"

namespace Yttrium
{

class Y_PRIVATE ThreadBufferBase::Private
{
public:

	Allocator    *allocator;
	StaticMutex   mutex;
	StaticSignal  read;
	StaticSignal  write;

	Private(Allocator *allocator)
		: allocator(allocator)
	{
	}
};

} // namespace Yttrium

#endif // __BASE_THREAD_BUFFER_H
