#include <cstdlib> // free, malloc, realloc
#include <new>     // bad_alloc

#include "heap_allocator.hpp"

// NOTE: The current implementation lamely uses CRT memory allocation routines.

namespace Yttrium
{

HeapAllocatorImpl::HeapAllocatorImpl() throw()
{
}

void *HeapAllocatorImpl::allocate(size_t size, size_t align, Difference *difference)
{
	void *pointer = malloc(size);
	if (!pointer)
	{
		throw std::bad_alloc();
	}

	Difference local_difference;

	if (!difference)
		difference = &local_difference;
	_status.allocate(difference->set(0, 0, true));

	return pointer;
}

void HeapAllocatorImpl::deallocate(void *pointer, Difference *difference) throw()
{
	if (pointer)
	{
		free(pointer);

		Difference local_difference;

		if (!difference)
			difference = &local_difference;
		_status.deallocate(difference->set(0, 0, false));
	}
}

void *HeapAllocatorImpl::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
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

	Difference local_difference;

	if (!difference)
		difference = &local_difference;
	_status.reallocate(difference->set(0, 0, true));

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
