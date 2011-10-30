#ifndef __BASE_HEAP_ALLOCATOR_HPP
#define __BASE_HEAP_ALLOCATOR_HPP

#include <Yttrium/allocator.hpp>

namespace Yttrium
{

class HeapAllocatorImpl
	: public HeapAllocator
{
public:

	HeapAllocatorImpl() throw();

public: // Allocator

	virtual void *allocate(size_t size, size_t align, Difference *difference);

	virtual void deallocate(void *pointer, Difference *difference) throw();

	virtual void *reallocate(void *pointer, size_t size, Movability movability, Difference *difference);

	virtual Status status() const throw();

private:

	AtomicStatus _status;
};

extern HeapAllocatorImpl *_heap_allocator;

} // namespace Yttrium

#endif // __BASE_HEAP_ALLOCATOR_HPP
