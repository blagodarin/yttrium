#include "heap_allocator.h"

#include <cstdlib>
#include <new>

namespace Yttrium
{
	void* HeapAllocator::do_allocate(size_t size, size_t alignment)
	{
		Y_UNUSED(alignment);
		const auto pointer = ::malloc(size);
		if (!pointer)
			throw std::bad_alloc();
		return pointer;
	}

	void HeapAllocator::do_deallocate(void* pointer, bool) noexcept
	{
		::free(pointer);
	}
}
