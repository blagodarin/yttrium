#include "heap_allocator.h"

#include <Yttrium/assert.h>

#include <cstdlib> // free, malloc, realloc

// NOTE: The current implementation lamely uses CRT memory allocation routines.

namespace Yttrium
{

void *HeapAllocator::allocate(size_t size, size_t align, Difference *difference)
{
	Y_UNUSED(align);

	void *pointer = malloc(size);

	if (Y_UNLIKELY(!pointer))
	{
		Y_ABORT("Out of memory"); // NOTE: Safe to continue.
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

void HeapAllocator::deallocate(void *pointer, Difference *difference)
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

void *HeapAllocator::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
{
	if (movability == MayNotMove)
	{
		return nullptr;
	}

	void *result = realloc(pointer, size);

	if (Y_UNLIKELY(!result))
	{
		Y_ABORT("Out of memory"); // NOTE: Safe to continue.
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

} // namespace Yttrium
