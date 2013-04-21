#include "system_allocator.h"

#include <Yttrium/assert.h>
#include <Yttrium/types.h>

#include <sys/mman.h> // mmap, mremap, munmap
#include <unistd.h>   // sysconf

// NOTE: We're using Linux-specific mremap() call here.
// Perhaps we should preprocess it out in non-Linux-compatible environments.

namespace Yttrium
{

SystemAllocatorImpl::SystemAllocatorImpl()
	: _page_size(::sysconf(_SC_PAGE_SIZE))
{
}

void *SystemAllocatorImpl::allocate(size_t size, size_t align, Difference *difference)
{
	Y_UNUSED(align);

	Y_ASSERT(size);

	size_t total_bytes = _page_size * ((ReservedSize + size + _page_size - 1) / _page_size);
	size_t allocated_bytes = total_bytes - ReservedSize;

	void *base = ::mmap(nullptr, total_bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (Y_UNLIKELY(base == MAP_FAILED))
	{
		Y_ABORT("Out of memory");
		return nullptr;
	}

	static_cast<size_t *>(base)[0] = total_bytes;
	static_cast<size_t *>(base)[1] = allocated_bytes;

	if (difference)
	{
		*difference = Difference(allocated_bytes, total_bytes, Difference::Increment);
	}

	return static_cast<char *>(base) + ReservedSize;
}

void SystemAllocatorImpl::deallocate(void *pointer, Difference *difference)
{
	if (Y_UNLIKELY(!pointer))
	{
		return;
	}

	void *base = static_cast<char *>(pointer) - ReservedSize;

	size_t total_bytes = static_cast<size_t *>(base)[0];
	size_t allocated_bytes = static_cast<size_t *>(base)[1];

	if (Y_UNLIKELY(::munmap(base, total_bytes)))
	{
		// This must be EINVAL, indicating that we've passed something wrong to munmap.

		Y_ABORT("Failed to deallocate memory");
	}

	if (difference)
	{
		*difference = Difference(allocated_bytes, total_bytes, Difference::Decrement);
	}
}

void *SystemAllocatorImpl::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
{
	Y_ASSERT(pointer);
	Y_ASSERT(size);

	void *base = static_cast<char *>(pointer) - ReservedSize;

	size_t total_bytes = static_cast<size_t *>(base)[0];
	size_t new_total_bytes = _page_size * ((ReservedSize + size + _page_size - 1) / _page_size);

	if (new_total_bytes == total_bytes)
	{
		if (difference)
		{
			*difference = Difference();
		}
		return pointer;
	}

	void *new_base = ::mremap(base, total_bytes, new_total_bytes, (movability == MayMove ? MREMAP_MAYMOVE : 0));

	if (Y_LIKELY(new_base == MAP_FAILED))
	{
		if (movability == MayMove)
		{
			Y_ABORT("Out of memory");
		}
		return nullptr;
	}

	static_cast<size_t *>(new_base)[0] = new_total_bytes;
	static_cast<size_t *>(new_base)[1] = new_total_bytes - ReservedSize;

	if (difference)
	{
		if (new_total_bytes > total_bytes)
		{
			size_t shift = new_total_bytes - total_bytes;
			*difference = Difference(shift, shift, Difference::Increment);
		}
		else
		{
			size_t shift = total_bytes - new_total_bytes;
			*difference = Difference(shift, shift, Difference::Decrement);
		}
	}

	return static_cast<char *>(new_base) + ReservedSize;
}

size_t SystemAllocatorImpl::lower_bound(size_t size) const
{
	Y_ASSERT(size);

	return _page_size * ((ReservedSize + size) / _page_size) - ReservedSize;
}

size_t SystemAllocatorImpl::upper_bound(size_t size) const
{
	Y_ASSERT(size);

	return _page_size * ((ReservedSize + size + _page_size - 1) / _page_size) - ReservedSize;
}

} // namespace Yttrium
