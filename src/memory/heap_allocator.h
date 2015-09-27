#ifndef _src_memory_heap_allocator_h_
#define _src_memory_heap_allocator_h_

#include <yttrium/allocator.h>

namespace Yttrium
{
	class HeapAllocator : public Allocator
	{
	private:

		void* do_allocate(size_t, size_t, Difference*) override;
		void do_deallocate(void*, Difference*) override;
		void* do_reallocate(void*, size_t, Movability, Difference*) override;
	};
}

#endif
