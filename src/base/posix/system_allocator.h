#ifndef __BASE_POSIX_SYSTEM_ALLOCATOR_H
#define __BASE_POSIX_SYSTEM_ALLOCATOR_H

#include <yttrium/allocator.h>

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

	enum: size_t
	{
		ReservedSize = 32, // This should be sufficient for both 32-bit and 64-bit memory.
	};

private:

	size_t _page_size;
};

} // namespace Yttrium

#endif // __BASE_POSIX_SYSTEM_ALLOCATOR_H
