#include <yttrium/memory/proxy_allocator.h>
#include <yttrium/static_string.h>
#include "memory/buffer_memory.h"
#include "memory/buffer_memory_tracker.h"
#include "memory/heap_allocator.h"

namespace Yttrium
{
#if Y_ENABLE_BUFFER_MEMORY_TRACKER
	BufferMemoryTracker _buffer_memory_tracker; // Must outlive BufferMemory.
#endif
	BufferMemory _buffer_memory;

	namespace
	{
		HeapAllocator _root_allocator;
		ProxyAllocator _default_allocator("heap"_s, _root_allocator);
	}

	Allocator* const DefaultAllocator = &_default_allocator;
}
