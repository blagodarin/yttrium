#include "heap_allocator.h"

#include <cstdlib>

namespace Yttrium
{
	void* HeapAllocator::allocate(size_t size, size_t align, Difference* difference)
	{
		Y_UNUSED(align);
		void* pointer = ::malloc(size);
		if (!pointer)
			throw std::bad_alloc();
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
			throw std::bad_alloc();
		if (difference)
			*difference = Difference(0, 0, Difference::Increment);
		return result;
	}
}
