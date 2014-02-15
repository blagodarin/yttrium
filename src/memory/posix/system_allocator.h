#ifndef __MEMORY_POSIX_SYSTEM_ALLOCATOR_H
#define __MEMORY_POSIX_SYSTEM_ALLOCATOR_H

#include <yttrium/system_allocator.h>

namespace Yttrium
{

class Y_PRIVATE SystemAllocator::Private
{
public: // SystemAllocator

	enum: size_t
	{
		ReservedSize = 32, // This should be sufficient for both 32-bit and 64-bit memory.
	};

public:

	Private();

	size_t page_size() const
	{
		return _page_size;
	}

	size_t total_size(size_t size) const
	{
		return ((size + ReservedSize + _page_size - 1) / _page_size) * _page_size;
	}

private:

	size_t _page_size;
};

} // namespace Yttrium

#endif // __MEMORY_POSIX_SYSTEM_ALLOCATOR_H
