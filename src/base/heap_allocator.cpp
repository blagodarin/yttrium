#include <cstdlib> // free, malloc, realloc
#include <new>     // bad_alloc

#include <Yttrium/allocators.hpp>
#include <Yttrium/assert.hpp>
#include <Yttrium/types.hpp>

#include "heap_allocator.hpp"

// NOTE: We should be GCC-compatible here.

// NOTE: The current implementation lamely uses CRT memory allocation routines.

namespace Yttrium
{

HeapAllocatorImpl::HeapAllocatorImpl() throw()
{
}

void HeapAllocatorImpl::initialize()
{
}

void *HeapAllocatorImpl::allocate(size_t size, size_t align)
{
	void *pointer = malloc(size);
	if (!pointer)
	{
		throw std::bad_alloc();
	}

	//__sync_add_and_fetch(&_status.total_bytes, total_bytes);
	//__sync_add_and_fetch(&_status.allocated_bytes, allocated_bytes);
	__sync_add_and_fetch(&_status.allocated_blocks, 1);

	__sync_add_and_fetch(&_status.allocations, 1);

	return pointer;
}

void HeapAllocatorImpl::deallocate(void *pointer) throw()
{
	if (pointer)
	{
		free(pointer);

		__sync_sub_and_fetch(&_status.allocated_blocks, 1);
		//__sync_sub_and_fetch(&_status.allocated_bytes, allocated_bytes);
		//__sync_sub_and_fetch(&_status.total_bytes, total_bytes);

		__sync_add_and_fetch(&_status.deallocations, 1);
	}
}

void *HeapAllocatorImpl::reallocate(void *pointer, size_t size, Movability movability)
{
	if (movability == MayNotMove)
	{
		return NULL;
	}

	void *new_pointer = realloc(pointer, size);
	if (!new_pointer)
	{
		throw std::bad_alloc();
	}

//	if (new_total_bytes > total_bytes)
//	{
//		size_t difference = new_total_bytes - total_bytes;
//
//		__sync_add_and_fetch(&_status.total_bytes, difference);
//		__sync_add_and_fetch(&_status.allocated_bytes, difference);
//	}
//	else
//	{
//		size_t difference = total_bytes - new_total_bytes;
//
//		__sync_sub_and_fetch(&_status.allocated_bytes, difference);
//		__sync_sub_and_fetch(&_status.total_bytes, difference);
//	}

	__sync_add_and_fetch(&_status.reallocations, 1);

	return new_pointer;
}

Allocator::Status HeapAllocatorImpl::status() const throw()
{
	return _status;
}

HeapAllocatorImpl *_heap_allocator = NULL;

HeapAllocator *HeapAllocator::instance() throw()
{
	return _heap_allocator;
}

} // namespace Yttrium
