#ifndef __MEMORY_HEAP_ALLOCATOR_H
#define __MEMORY_HEAP_ALLOCATOR_H

#include <yttrium/allocator.h>

namespace Yttrium
{

class HeapAllocator
	: public Allocator
{
public:

	HeapAllocator(Allocator *parent)
	{
		Y_UNUSED(parent);
	}

public: // Allocator

	void* allocate(size_t size, size_t align, Difference* difference) noexcept override;
	void deallocate(void *pointer, Difference *difference) noexcept override;
	void* reallocate(void *pointer, size_t size, Movability movability, Difference* difference) noexcept override;
};

} // namespace Yttrium

#endif // __MEMORY_HEAP_ALLOCATOR_H
