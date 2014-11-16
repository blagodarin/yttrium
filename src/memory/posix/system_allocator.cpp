#include "system_allocator.h"

#include <yttrium/assert.h>
#include <yttrium/types.h>
#include <yttrium/utils.h>

#include <errno.h>    // errno
#include <sys/mman.h> // mmap, mremap, munmap
#include <unistd.h>   // sysconf

// NOTE: We're using Linux-specific mremap() call here.
// Perhaps we should preprocess it out in non-Linux-compatible environments.

namespace Yttrium
{

SystemAllocator::Private::Private()
	: _page_size(::sysconf(_SC_PAGE_SIZE))
{
}

void *SystemAllocator::allocate(size_t size, size_t align, Difference *difference)
{
	Y_ASSERT(size > 0);
	Y_ASSERT(align == 0 || (is_power_of_2(align) && align <= Private::ReservedSize));

	const size_t total_size = _private->total_size(size);

	void *base = ::mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (base == MAP_FAILED)
	{
		Y_ABORT("Out of memory");
		return nullptr;
	}

	static_cast<size_t *>(base)[0] = total_size;
	static_cast<size_t *>(base)[1] = size;

	if (difference)
		*difference = Difference(size, total_size, Difference::Increment);

	return static_cast<char *>(base) + Private::ReservedSize;
}

void SystemAllocator::deallocate(void *pointer, Difference *difference)
{
	if (!pointer)
		return;

	void *base = static_cast<char *>(pointer) - Private::ReservedSize;

	const size_t total_size = static_cast<size_t *>(base)[0];
	const size_t size = static_cast<size_t *>(base)[1];

	if (::munmap(base, total_size))
	{
		Y_ASSERT(errno == EINVAL); // This means invalid pointer value, anything else should mean internal system error.
		Y_ABORT("Failed to deallocate memory");
	}

	if (difference)
		*difference = Difference(size, total_size, Difference::Decrement);
}

void *SystemAllocator::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
{
	Y_ASSERT(pointer);
	Y_ASSERT(size);

	void *base = static_cast<char *>(pointer) - Private::ReservedSize;

	const size_t old_total_size = static_cast<size_t *>(base)[0];
	const size_t old_size = static_cast<size_t *>(base)[1];
	const size_t total_size = _private->total_size(size);

	if (total_size != old_total_size)
	{
		base = ::mremap(base, old_total_size, total_size, (movability == MayMove ? MREMAP_MAYMOVE : 0));

		if (base == MAP_FAILED)
		{
			if (movability == MayMove)
				Y_ABORT("Out of memory");
			return nullptr;
		}

		static_cast<size_t *>(base)[0] = total_size;
		static_cast<size_t *>(base)[1] = size;
	}

	if (difference)
	{
		if (size >= old_size)
			*difference = Difference(size - old_size, total_size - old_total_size, Difference::Increment);
		else
			*difference = Difference(old_size - size, old_total_size - total_size, Difference::Decrement);
	}

	return static_cast<char *>(base) + Private::ReservedSize;
}

} // namespace Yttrium
