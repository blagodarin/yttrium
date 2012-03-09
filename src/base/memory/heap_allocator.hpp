#ifndef __BASE_MEMORY_HEAP_ALLOCATOR_HPP
#define __BASE_MEMORY_HEAP_ALLOCATOR_HPP

#include <Yttrium/allocator.hpp>

namespace Yttrium
{

extern HeapAllocator *_heap_allocator;

class HeapAllocatorImpl
	: public HeapAllocator
{
public:

	HeapAllocatorImpl()
	{
		_heap_allocator = this;
	}

	virtual ~HeapAllocatorImpl() noexcept
	{
		_heap_allocator = nullptr;
	}

public: // Allocator

	virtual void *allocate(size_t size, size_t align, Difference *difference) noexcept;

	virtual void deallocate(void *pointer, Difference *difference) noexcept;

	virtual void *reallocate(void *pointer, size_t size, Movability movability, Difference *difference) noexcept;
};

} // namespace Yttrium

#endif // __BASE_MEMORY_HEAP_ALLOCATOR_HPP
