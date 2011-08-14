#ifndef __BASE_HEAP_ALLOCATOR_HPP
#define __BASE_HEAP_ALLOCATOR_HPP

#include <Yttrium/allocators.hpp>

namespace Yttrium
{

class HeapAllocatorImpl
	: public HeapAllocator
{
public:

	HeapAllocatorImpl() throw();

public: // Allocator

	virtual void *allocate(size_t size, size_t align);

	virtual void deallocate(void *pointer) throw();

	virtual void *reallocate(void *pointer, size_t size, Movability movability);

	virtual Status status() const throw();

private:

	Status _status;
};

extern HeapAllocatorImpl *_heap_allocator;

} // namespace Yttrium

#endif // __BASE_HEAP_ALLOCATOR_HPP
