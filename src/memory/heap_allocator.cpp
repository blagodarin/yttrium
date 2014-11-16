#include "heap_allocator.h"

#include <yttrium/assert.h>

#include <cstdlib> // free, malloc, realloc

// TODO: Consider implementing custom allocator instead of using the standard one.

namespace Yttrium
{

void *HeapAllocator::allocate(size_t size, size_t align, Difference* difference)
{
	Y_UNUSED(align);

	void *pointer = ::malloc(size);

	if (!pointer)
	{
		Y_ABORT("Out of memory"); // NOTE: Safe to continue.
		return nullptr;
	}

	if (difference)
		*difference = Difference(0, 0, Difference::Increment);

	return pointer;
}

void HeapAllocator::deallocate(void* pointer, Difference* difference)
{
	if (!pointer)
		return;

	::free(pointer);

	if (difference)
		*difference = Difference(0, 0, Difference::Decrement);
}

void* HeapAllocator::reallocate(void* pointer, size_t size, Movability movability, Difference* difference)
{
	if (movability != MayMove)
		return nullptr;

	void* result = ::realloc(pointer, size);

	if (!result)
	{
		Y_ABORT("Out of memory");
		return nullptr;
	}

	if (difference)
		*difference = Difference(0, 0, Difference::Increment);

	return result;
}

} // namespace Yttrium
