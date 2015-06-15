#ifndef __MEMORY_HEAP_ALLOCATOR_H
#define __MEMORY_HEAP_ALLOCATOR_H

#include <yttrium/allocator.h>

namespace Yttrium
{
	class HeapAllocator : public Allocator
	{
	public:

		void* allocate(size_t size, size_t align, Difference* difference) override;
		void deallocate(void* pointer, Difference* difference) override;
		void* reallocate(void* pointer, size_t size, Movability movability, Difference* difference) override;
	};
}

#endif // __MEMORY_HEAP_ALLOCATOR_H
