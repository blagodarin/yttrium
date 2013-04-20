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

	size_t total_bytes = _page_size * ((reserved_size + size + _page_size - 1) / _page_size);
	size_t allocated_bytes = total_bytes - reserved_size;

	void *base = ::mmap(nullptr, total_bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (Y_UNLIKELY(base == MAP_FAILED))
	{
		Y_ABORT("Out of memory"); // NOTE: Safe to continue.
		return nullptr;
	}

	static_cast<size_t *>(base)[0] = total_bytes;
	static_cast<size_t *>(base)[1] = allocated_bytes;

	Difference local_difference;

	if (!difference)
	{
		difference = &local_difference;
	}
	*difference = Difference(allocated_bytes, total_bytes, Difference::Increment);
	_status.deallocate(*difference);

	return static_cast<char *>(base) + reserved_size;
}

void SystemAllocatorImpl::deallocate(void *pointer, Difference *difference)
{
	if (Y_LIKELY(pointer))
	{
		void *base = static_cast<char *>(pointer) - reserved_size;

		size_t total_bytes = static_cast<size_t *>(base)[0];
		size_t allocated_bytes = static_cast<size_t *>(base)[1];

		::munmap(base, total_bytes);

		Difference local_difference;

		if (!difference)
		{
			difference = &local_difference;
		}
		*difference = Difference(allocated_bytes, total_bytes, Difference::Decrement);
		_status.deallocate(*difference);
	}
}

void *SystemAllocatorImpl::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
{
	Y_ASSERT(pointer);
	Y_ASSERT(size);

	void *base = static_cast<char *>(pointer) - reserved_size;

	size_t total_bytes = static_cast<size_t *>(base)[0];

	size_t new_total_bytes = _page_size * ((reserved_size + size + _page_size - 1) / _page_size);

	Difference local_difference;

	if (!difference)
	{
		difference = &local_difference;
	}

	if (new_total_bytes == total_bytes)
	{
		*difference = Difference(0, 0, Difference::Increment);
		return pointer;
	}

	void *new_base = ::mremap(base, total_bytes, new_total_bytes, (movability == MayMove ? MREMAP_MAYMOVE : 0));
	if (Y_UNLIKELY(new_base == MAP_FAILED))
	{
		Y_ABORT_IF(movability == MayMove, "Out of memory"); // NOTE: Safe to continue.
		return nullptr;
	}

	static_cast<size_t *>(new_base)[0] = new_total_bytes;
	static_cast<size_t *>(new_base)[1] = new_total_bytes - reserved_size;

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
	_status.deallocate(*difference);

	return static_cast<char *>(new_base) + reserved_size;
}

size_t SystemAllocatorImpl::lower_bound(size_t size) const
{
	Y_ASSERT(size);

	return _page_size * ((reserved_size + size) / _page_size) - reserved_size;
}

size_t SystemAllocatorImpl::upper_bound(size_t size) const
{
	Y_ASSERT(size);

	return _page_size * ((reserved_size + size + _page_size - 1) / _page_size) - reserved_size;
}

const size_t SystemAllocatorImpl::reserved_size = 32; // This should be sufficient for both 32-bit and 64-bit memory.

} // namespace Yttrium
