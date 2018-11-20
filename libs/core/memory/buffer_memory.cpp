#include "buffer_memory.h"

#include <yttrium/utils/numeric.h>
#include "memory.h"

#if Y_ENABLE_BUFFER_MEMORY_TRACKING
#	include "buffer_memory_tracker.h"
#endif

#include <cassert>
#include <cstring>
#include <new>

namespace
{
	void* allocate_big_block(size_t size)
	{
		const auto data = Yttrium::pages_allocate(size);
		if (!data)
			throw std::bad_alloc{};
#if Y_ENABLE_BUFFER_MEMORY_TRACKING
		Yttrium::_buffer_memory_tracker.track_system_allocation(size);
#endif
		return data;
	}

	size_t level_from_capacity(size_t capacity) noexcept
	{
		assert(Yttrium::is_power_of_2(capacity));
		size_t level = 0;
		for (auto i = capacity; i > 1; i >>= 1)
			++level;
		assert(size_t{ 1 } << level == capacity);
		return level;
	}
}

namespace Yttrium
{
	BufferMemory::~BufferMemory()
	{
#if Y_ENABLE_BUFFER_MEMORY_TRACKING && Y_ENABLE_BUFFER_MEMORY_DEBUGGING
		std::map<size_t, size_t> free_block_count;
		{
			std::lock_guard<std::mutex> lock{ _small_blocks_mutex };
			for (auto i = ::level_from_capacity(capacity_for_size(1)); i <= MaxSmallBlockLevel; ++i)
			{
				size_t count = 0;
				for (void* block = _small_blocks[i]; block; block = *reinterpret_cast<void**>(block))
					++count;
				if (count > 0)
					free_block_count.emplace(1 << i, count);
			}
		}
		_buffer_memory_tracker.print_state(free_block_count);
#endif
	}

	void* BufferMemory::allocate(size_t capacity)
	{
		assert(capacity > 0 && capacity == capacity_for_size(capacity));
		void* data = nullptr;
		if (capacity > MaxSmallBlockSize)
		{
			data = ::allocate_big_block(capacity);
		}
		else
		{
			const auto level = ::level_from_capacity(capacity);
			{
				std::lock_guard<std::mutex> lock{ _small_blocks_mutex };
				data = _small_blocks[level];
				if (data)
					_small_blocks[level] = *reinterpret_cast<void**>(data);
				else
				{
					auto i = level + 1;
					for (; i <= MaxSmallBlockLevel; ++i)
					{
						if (data = _small_blocks[i]; data)
						{
							_small_blocks[i] = *reinterpret_cast<void**>(data);
							break;
						}
					}
					if (!data)
					{
						// TODO: Try to merge smaller blocks before allocating a new big one.
						assert(size_t{ 1 } << i == 2 * MaxSmallBlockSize);
						data = ::allocate_big_block(2 * MaxSmallBlockSize);
					}
					do
					{
						--i;
						*reinterpret_cast<void**>(data) = _small_blocks[i];
						_small_blocks[i] = data;
						data = static_cast<uint8_t*>(data) + (size_t{ 1 } << i);
					} while (i > level);
				}
			}
		}
#if Y_ENABLE_BUFFER_MEMORY_TRACKING
		_buffer_memory_tracker.track_capacity_allocation(capacity);
#endif
		return data;
	}

	void BufferMemory::deallocate(void* data, size_t capacity) noexcept
	{
		assert(data);
		assert(capacity > 0 && capacity == capacity_for_size(capacity));
		if (capacity > MaxSmallBlockSize)
		{
			pages_deallocate(data, capacity);
#if Y_ENABLE_BUFFER_MEMORY_TRACKING
			_buffer_memory_tracker.track_system_deallocation(capacity);
#endif
		}
		else
		{
			const auto level = ::level_from_capacity(capacity);
			{
				std::lock_guard<std::mutex> lock{ _small_blocks_mutex };
				*reinterpret_cast<void**>(data) = _small_blocks[level];
				_small_blocks[level] = data;
			}
		}
#if Y_ENABLE_BUFFER_MEMORY_TRACKING
		_buffer_memory_tracker.track_capacity_deallocation(capacity);
#endif
	}

	void* BufferMemory::reallocate(void* old_data, size_t old_capacity, size_t new_capacity, size_t old_size)
	{
		assert(old_data);
		assert(old_capacity > 0 && old_capacity == capacity_for_size(old_capacity));
		assert(new_capacity > 0 && new_capacity == capacity_for_size(new_capacity));
		assert(old_capacity != new_capacity);
		if (old_capacity > MaxSmallBlockSize && new_capacity > MaxSmallBlockSize)
		{
			const auto new_data = pages_reallocate(old_data, old_capacity, new_capacity);
			if (!new_data)
				throw std::bad_alloc{};
#if Y_ENABLE_BUFFER_MEMORY_TRACKING
			_buffer_memory_tracker.track_system_reallocation(old_capacity, new_capacity);
			_buffer_memory_tracker.track_capacity_change(old_capacity, new_capacity);
			_buffer_memory_tracker.track_reallocation(); // TODO: Perhaps pure shrinking reallocations should be counted differently.
#endif
			return new_data;
		}
		else
		{
			// This situation is rare, so let's do something simple.
			// TODO: Remap the first part of the big allocation into the small free block if one is available.
			const auto new_data = allocate(new_capacity);
			if (old_size > 0)
				::memcpy(new_data, old_data, old_size);
			deallocate(old_data, old_capacity);
#if Y_ENABLE_BUFFER_MEMORY_TRACKING
			_buffer_memory_tracker.track_reallocation();
#endif
			return new_data;
		}
	}

	size_t BufferMemory::capacity_for_size(size_t size) noexcept
	{
		const auto granularity_mask = granularity() - 1;
		auto capacity = (size + granularity_mask) & ~granularity_mask;
		return capacity > MaxSmallBlockSize ? capacity : next_power_of_2(capacity);
	}

	size_t BufferMemory::granularity() noexcept
	{
		static const auto page_size = pages_granularity();
		assert(is_power_of_2(page_size));
		return page_size;
	}
}
