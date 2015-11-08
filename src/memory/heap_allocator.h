#ifndef _src_memory_heap_allocator_h_
#define _src_memory_heap_allocator_h_

#include <yttrium/memory/allocator.h>

namespace Yttrium
{
	class HeapAllocator : public Allocator
	{
	private:

		void* do_allocate(size_t, size_t) override;
		void do_deallocate(void*, bool) noexcept override;
	};
}

#endif
