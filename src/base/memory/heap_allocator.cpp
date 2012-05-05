#include "heap_allocator.hpp"

#include <cstdlib> // free, malloc, realloc

#include <Yttrium/assert.hpp>

// NOTE: The current implementation lamely uses CRT memory allocation routines.

namespace Yttrium
{

HeapAllocator *_heap_allocator = nullptr;

void *HeapAllocatorImpl::allocate(size_t size, size_t align, Difference *difference)
{
	void *pointer = malloc(size);

	if (Y_UNLIKELY(!pointer))
	{
		Y_ABORT("Out of memory");
		return nullptr;
	}

	Difference local_difference;

	if (!difference)
	{
		difference = &local_difference;
	}

	*difference = Difference(0, 0, Difference::Increment);
	_status.allocate(*difference);

	return pointer;
}

void HeapAllocatorImpl::deallocate(void *pointer, Difference *difference)
{
	if (Y_LIKELY(pointer))
	{
		free(pointer);

		Difference local_difference;

		if (!difference)
		{
			difference = &local_difference;
		}

		*difference = Difference(0, 0, Difference::Decrement);
		_status.deallocate(*difference);
	}
}

void *HeapAllocatorImpl::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
{
	if (movability == MayNotMove)
	{
		return nullptr;
	}

	void *result = realloc(pointer, size);

	if (Y_UNLIKELY(!result))
	{
		Y_ABORT("Out of memory");
		return nullptr;
	}

	Difference local_difference;

	if (!difference)
	{
		difference = &local_difference;
	}

	*difference = Difference(0, 0, Difference::Increment);
	_status.reallocate(*difference);

	return result;
}

HeapAllocator *HeapAllocator::instance()
{
	return _heap_allocator;
}

} // namespace Yttrium
