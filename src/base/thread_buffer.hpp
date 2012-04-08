#ifndef __BASE_THREAD_BUFFER_HPP
#define __BASE_THREAD_BUFFER_HPP

#include <Yttrium/thread_buffer.hpp>

#include "mutex.hpp"
#include "signal.hpp"

namespace Yttrium
{

class ThreadBufferBase::Private
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

#endif // __BASE_THREAD_BUFFER_HPP
