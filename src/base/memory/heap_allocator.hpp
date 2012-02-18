#ifndef __BASE_MEMORY_HEAP_ALLOCATOR_HPP
#define __BASE_MEMORY_HEAP_ALLOCATOR_HPP

#include <Yttrium/allocator.hpp>

namespace Yttrium
{

class HeapAllocatorImpl
	: public HeapAllocator
{
public:

	HeapAllocatorImpl();

public: // Allocator

	virtual void *allocate(size_t size, size_t align, Difference *difference) noexcept;

	virtual void deallocate(void *pointer, Difference *difference) noexcept;

	virtual void *reallocate(void *pointer, size_t size, Movability movability, Difference *difference) noexcept;
};

extern HeapAllocatorImpl *_heap_allocator;

} // namespace Yttrium

#endif // __BASE_MEMORY_HEAP_ALLOCATOR_HPP
