#ifndef __BASE_MEMORY_HEAP_ALLOCATOR_H
#define __BASE_MEMORY_HEAP_ALLOCATOR_H

#include <Yttrium/allocator.h>

namespace Yttrium
{

class HeapAllocator: public Allocator
{
public:

	HeapAllocator(Allocator *parent)
	{
		Y_UNUSED(parent);
	}

	virtual ~HeapAllocator() noexcept
	{
	}

public: // Allocator

	virtual void *allocate(size_t size, size_t align, Difference *difference) noexcept;

	virtual void deallocate(void *pointer, Difference *difference) noexcept;

	virtual void *reallocate(void *pointer, size_t size, Movability movability, Difference *difference) noexcept;
};

} // namespace Yttrium

#endif // __BASE_MEMORY_HEAP_ALLOCATOR_H
