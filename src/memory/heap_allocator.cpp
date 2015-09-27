#include "heap_allocator.h"

#include <cstdlib>

namespace Yttrium
{
	void* HeapAllocator::do_allocate(size_t size, size_t alignment, Difference* difference)
	{
		Y_UNUSED(alignment);
		const auto pointer = ::malloc(size);
		if (!pointer)
			throw std::bad_alloc();
		if (difference)
			*difference = Difference(0, 0, Difference::Increment);
		return pointer;
	}

	void HeapAllocator::do_deallocate(void* pointer, Difference* difference)
	{
		::free(pointer);
		if (difference)
			*difference = Difference(0, 0, Difference::Decrement);
	}

	void* HeapAllocator::do_reallocate(void* pointer, size_t size, Movability movability, Difference* difference)
	{
		if (movability != MayMove)
			return nullptr;
		const auto new_pointer = ::realloc(pointer, size);
		if (!new_pointer)
			throw std::bad_alloc();
		if (difference)
			*difference = Difference(0, 0, Difference::Increment);
		return new_pointer;
	}
}
