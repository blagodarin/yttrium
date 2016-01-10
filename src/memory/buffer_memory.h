#ifndef _src_memory_buffer_memory_h_
#define _src_memory_buffer_memory_h_

#include <cstddef>

namespace Yttrium
{
	void* buffer_memory_allocate(size_t capacity);
	size_t buffer_memory_capacity(size_t size) noexcept;
	void buffer_memory_deallocate(void* data, size_t capacity) noexcept;
	size_t buffer_memory_granularity() noexcept;
	size_t buffer_memory_max_total_capacity() noexcept;
	void* buffer_memory_reallocate(void* old_data, size_t old_capacity, size_t new_capacity, size_t old_size);
	size_t buffer_memory_total_capacity() noexcept;
}

#endif
