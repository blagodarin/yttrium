#ifndef _src_memory_buffer_memory_h_
#define _src_memory_buffer_memory_h_

#include <cstddef>

#define Y_ENABLE_BUFFER_MEMORY_TRACKING 1 // Disable for profiling purposes only.

namespace Yttrium
{
	void* buffer_memory_allocate(size_t capacity);
	size_t buffer_memory_capacity(size_t size) noexcept;
	void buffer_memory_deallocate(void* data, size_t capacity) noexcept;
	size_t buffer_memory_granularity() noexcept;
	void* buffer_memory_reallocate(void* old_data, size_t old_capacity, size_t new_capacity, size_t old_size);

#if Y_ENABLE_BUFFER_MEMORY_TRACKING
	size_t buffer_memory_max_total_size() noexcept;
	size_t buffer_memory_total_size() noexcept;
	void buffer_memory_track_size_allocation(size_t size) noexcept;
	void buffer_memory_track_size_change(size_t old_size, size_t new_size) noexcept;
	void buffer_memory_track_size_deallocation(size_t size) noexcept;
#endif
}

#endif
