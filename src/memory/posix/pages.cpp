#include "../pages.h"

#include <yttrium/utils.h>

#include <cassert>
#include <cstdlib>

#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

namespace Yttrium
{
	const size_t _page_size_mask = []()
	{
		const auto page_size = ::sysconf(_SC_PAGE_SIZE);
		assert(page_size > 0);
		assert(is_power_of_2(page_size));
		return page_size - 1;
	}();

	void* pages_allocate(size_t size) noexcept
	{
		assert(size > 0 && size == pages_size(size));
		const auto result = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (result != MAP_FAILED)
			return result;
		if (errno != ENOMEM)
			return nullptr;
		::abort();
	}

	void pages_deallocate(void* pointer, size_t size) noexcept
	{
		assert(pointer);
		assert(size > 0 && size == pages_size(size));
		if (::munmap(pointer, size) != 0)
			::abort();
	}

	void* pages_reallocate(void* old_pointer, size_t old_size, size_t new_size) noexcept
	{
		assert(old_pointer);
		assert(old_size > 0 && old_size == pages_size(old_size));
		assert(new_size > 0 && new_size == pages_size(new_size));
		const auto new_pointer = ::mremap(old_pointer, old_size, new_size, MREMAP_MAYMOVE); // TODO: Make portable equivalent.
		if (new_pointer != MAP_FAILED)
			return new_pointer;
		if (errno != ENOMEM)
			return nullptr;
		::abort();
	}

	size_t pages_size(size_t size) noexcept
	{
		return (size + _page_size_mask) & ~_page_size_mask;
	}
}
