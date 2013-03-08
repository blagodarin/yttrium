#ifndef __BASE_POSIX_SYSTEM_ALLOCATOR_H
#define __BASE_POSIX_SYSTEM_ALLOCATOR_H

#include <Yttrium/allocator.h>

namespace Yttrium
{

class SystemAllocatorImpl
	: public SystemAllocator
{
public:

	SystemAllocatorImpl();

public: // Allocator

	void *allocate(size_t size, size_t align, Difference *difference) noexcept override;

	void deallocate(void *pointer, Difference *difference) noexcept override;

	void *reallocate(void *pointer, size_t size, Movability movability, Difference *difference) noexcept override;

public: // SystemAllocator

	size_t lower_bound(size_t size) const noexcept override;

	size_t upper_bound(size_t size) const noexcept override;

private:

	static const size_t reserved_size;

private:

	size_t _page_size;
};

} // namespace Yttrium

#endif // __BASE_POSIX_SYSTEM_ALLOCATOR_H
