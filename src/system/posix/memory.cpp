#include "../memory.h"

#include <yttrium/utils.h>
#include "../../config.h"

#include <cstdlib>
#include <cstring>

#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

namespace Yttrium
{
	void* pages_allocate(size_t size) noexcept
	{
		const auto result = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (result != MAP_FAILED)
			return result;
		if (errno != ENOMEM)
			return nullptr;
		::abort();
	}

	void pages_deallocate(void* pointer, size_t size) noexcept
	{
		if (::munmap(pointer, size) != 0)
			::abort();
	}

	size_t pages_granularity() noexcept
	{
		const auto page_size = ::sysconf(_SC_PAGESIZE);
		if (page_size <= 0)
			::abort();
		return page_size;
	}

	void* pages_reallocate(void* old_pointer, size_t old_size, size_t new_size) noexcept
	{
#ifdef Y_IS_LINUX
		const auto new_pointer = ::mremap(old_pointer, old_size, new_size, MREMAP_MAYMOVE);
		if (new_pointer != MAP_FAILED)
			return new_pointer;
#else
		const auto new_pointer = ::mmap(nullptr, new_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (new_pointer != MAP_FAILED)
		{
			std::memcpy(new_pointer, old_pointer, min(old_size, new_size));
			if (::munmap(old_pointer, old_size) != 0)
				::abort();
			return new_pointer;
		}
#endif
		if (errno != ENOMEM && old_size < new_size)
			return nullptr;
		::abort();
	}
}
