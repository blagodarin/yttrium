#ifndef _src_system_memory_h_
#define _src_system_memory_h_

#include <cstddef>

namespace Yttrium
{
	void* pages_allocate(size_t) noexcept;
	void pages_deallocate(void*, size_t) noexcept;
	size_t pages_granularity() noexcept;
	void* pages_reallocate(void*, size_t, size_t) noexcept;
}

#endif
