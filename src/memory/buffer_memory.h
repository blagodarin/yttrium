#ifndef _src_memory_buffer_memory_h_
#define _src_memory_buffer_memory_h_

#include <yttrium/global.h>

#include <atomic>
#include <cstddef>

#define Y_BUFFER_TRACK_ALLOCATION_COUNT Y_IS_DEBUG
#define Y_BUFFER_TRACK_WASTED_MEMORY Y_IS_DEBUG

namespace Yttrium
{
	struct BufferMemoryStatus
	{
		struct AtomicPair
		{
			std::atomic<size_t> _value{0};
			std::atomic<size_t> _max_value{0};

			void add(size_t difference) noexcept
			{
				auto old_value = _value.fetch_add(difference);
				const auto new_value = old_value + difference;
				while (!_max_value.compare_exchange_strong(old_value, new_value))
					if (old_value >= new_value)
						break;
			}
		};

		AtomicPair _allocated;
	#if Y_BUFFER_TRACK_ALLOCATION_COUNT
		AtomicPair _allocations;
		std::atomic<size_t> _total_allocations{0};
		std::atomic<size_t> _total_reallocations{0};
	#endif
	#if Y_BUFFER_TRACK_WASTED_MEMORY
		AtomicPair _wasted;
	#endif

		~BufferMemoryStatus();
	};

	extern BufferMemoryStatus _buffer_memory_status;

	void* buffer_memory_allocate(size_t capacity);
	size_t buffer_memory_capacity(size_t size) noexcept;
	void buffer_memory_deallocate(void* data, size_t capacity) noexcept;
	size_t buffer_memory_granularity() noexcept;
	void* buffer_memory_reallocate(void* old_data, size_t old_capacity, size_t new_capacity, size_t old_size);
}

#endif
