#ifndef __BASE_POSIX_SYSTEM_ALLOCATOR_HPP
#define __BASE_POSIX_SYSTEM_ALLOCATOR_HPP

#include <Yttrium/allocator.hpp>

namespace Yttrium
{

class SystemAllocatorImpl
	: public SystemAllocator
{
public:

	SystemAllocatorImpl() throw();

public: // Allocator

	virtual void *allocate(size_t size, size_t align, Difference *difference);

	virtual void deallocate(void *pointer, Difference *difference) throw();

	virtual void *reallocate(void *pointer, size_t size, Movability movability, Difference *difference);

	virtual Status status() const throw();

public: // SystemAllocator

	virtual size_t lower_bound(size_t size) const throw();

	virtual size_t upper_bound(size_t size) const throw();

private:

	static const size_t reserved_size;

private:

	AtomicStatus _status;
	size_t       _page_size;
};

} // namespace Yttrium

#endif // __BASE_POSIX_SYSTEM_ALLOCATOR_HPP
