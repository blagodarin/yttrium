#include <yttrium/memory/global.h>
#include "memory/buffer_memory.h"
#include "memory/buffer_memory_tracker.h"
#include "memory/heap_allocator.h"

namespace Yttrium
{
	namespace
	{
		class : public Allocator
		{
			void* do_allocate(size_t, size_t) override { std::abort(); }
			void do_deallocate(void*, bool) noexcept override { std::abort(); }
		} _no_allocator;
	}
	Allocator& NoAllocator = _no_allocator;

#if Y_ENABLE_BUFFER_MEMORY_TRACKING
	BufferMemoryTracker _buffer_memory_tracker; // Must outlive BufferMemory.
#endif
	BufferMemory _buffer_memory;

	HeapAllocator _heap_allocator;
	Allocator* const DefaultAllocator = &_heap_allocator;
}
