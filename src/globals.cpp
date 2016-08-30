#include <yttrium/memory/named_allocator.h>
#include "base/log.h"
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
		NamedAllocator _default_allocator("heap"_s, _root_allocator);

		class : public Allocator
		{
			void* do_allocate(size_t, size_t) override { std::abort(); }
			void do_deallocate(void*, bool) noexcept override { std::abort(); }
		} _no_allocator;
	}

	Allocator* const DefaultAllocator = &_default_allocator;
	Allocator& NoAllocator = _no_allocator;

	namespace
	{
		NamedAllocator _log_allocator("log"_s, _default_allocator);
	}

	LogManager _log_manager(_log_allocator);
}
