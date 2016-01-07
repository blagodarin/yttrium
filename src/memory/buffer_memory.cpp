#include "buffer_memory.h"

#include <yttrium/utils.h>
#include "pages.h"

#include <cassert>
#include <cstring>
#include <mutex>
#include <new>

#define PRINT_TRACKING_INFO Y_IS_DEBUG

#if PRINT_TRACKING_INFO
	#include <iostream>
	#include <sstream>
#endif

namespace Yttrium
{
	namespace
	{
		constexpr size_t MaxSmallBlockLevel = 19;
		constexpr size_t MaxSmallBlockSize = 1 << MaxSmallBlockLevel;

		void* _small_blocks[MaxSmallBlockLevel + 1];
		std::mutex _small_blocks_mutex;

		void* allocate_big_block(size_t size)
		{
			const auto data = pages_allocate(size);
			if (!data)
				throw std::bad_alloc();
			_buffer_memory_status._total_allocated.add(size);
		#if Y_BUFFER_TRACK_ALLOCATION_COUNT
			++_buffer_memory_status._total_system_allocations;
		#endif
			return data;
		}

		size_t level_from_capacity(size_t capacity) noexcept
		{
			assert(is_power_of_2(capacity));
			size_t level = 0;
			for (auto i = capacity; i > 1; i >>= 1)
				++level;
			assert(size_t{1} << level == capacity);
			return level;
		}
	}

	BufferMemoryStatus::~BufferMemoryStatus()
	{
	#if PRINT_TRACKING_INFO
		const auto human_readable_size = [](size_t size)
		{
			std::ostringstream stream;
			if (size < 1024)
				stream << size << " B";
			else if (size < 1024 * 1024)
				stream << size / 1024. << " KiB";
			else
				stream << size / 1024. / 1024. << " MiB";
			return stream.str();
		};

		const auto max_allocated = _total_allocated._max_value.load();
		if (!max_allocated)
			return;
		std::cerr << "(DEBUG) Buffer memory statistics:";
		std::cerr << "\n(DEBUG)  * max_total_allocated : " << human_readable_size(_total_allocated._max_value);
		std::cerr << "\n(DEBUG)  * max_total_capacity : " << human_readable_size(_total_capacity._max_value);
	#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
		std::cerr << "\n(DEBUG)  * max_total_capacity_wasted : " << human_readable_size(_total_capacity_wasted._max_value);
	#endif
	#if Y_BUFFER_TRACK_MAX_CAPACITY
		std::cerr << "\n(DEBUG)  * max_capacity : " << human_readable_size(_max_capacity._value);
	#endif
	#if Y_BUFFER_TRACK_ALLOCATION_COUNT
		std::cerr << "\n(DEBUG)  * max_allocations : " << _allocations._max_value;
		std::cerr << "\n(DEBUG)  * total_system_allocations : " << _total_system_allocations;
		std::cerr << "\n(DEBUG)  * total_allocations : " << _total_allocations;
		std::cerr << "\n(DEBUG)  * total_reallocations : " << _total_reallocations;
	#endif
	#if Y_IS_DEBUG
		std::cerr << "\n(DEBUG)  * remaining_blocks : " << human_readable_size(_total_allocated._value);
		{
			std::lock_guard<std::mutex> lock(_small_blocks_mutex);
			for (auto i = level_from_capacity(buffer_memory_capacity(1)); i <= MaxSmallBlockLevel; ++i)
			{
				std::cerr << "\n(DEBUG)     - " << human_readable_size(1 << i) << " : ";
				size_t block_count = 0;
				for (void* block = _small_blocks[i]; block; block = *reinterpret_cast<void**>(block))
					++block_count;
				std::cerr << block_count;
			}
		}
	#endif
		std::cerr << std::endl;
		const auto total_capacity = _total_capacity._value.load();
		if (total_capacity > 0)
		{
			std::cerr << "(ERROR) Buffer memory leaked: " << human_readable_size(total_capacity);
		#if Y_BUFFER_TRACK_ALLOCATION_COUNT
			std::cerr << " (in " << _allocations._value << " allocations)";
		#endif
			std::cerr << std::endl;
		}
	#endif
	}

	BufferMemoryStatus _buffer_memory_status;

	void* buffer_memory_allocate(size_t capacity)
	{
		assert(capacity > 0 && capacity == buffer_memory_capacity(capacity));
		void* data = nullptr;
		if (capacity > MaxSmallBlockSize)
		{
			data = allocate_big_block(capacity);
		}
		else
		{
			const auto level = level_from_capacity(capacity);
			{
				std::lock_guard<std::mutex> lock(_small_blocks_mutex);
				data = _small_blocks[level];
				if (data)
					_small_blocks[level] = *reinterpret_cast<void**>(data);
				else
				{
					auto i = level + 1;
					for (; i <= MaxSmallBlockLevel; ++i)
					{
						data = _small_blocks[i];
						if (data)
						{
							_small_blocks[i] = *reinterpret_cast<void**>(data);
							break;
						}
					}
					if (!data)
					{
						assert(1 << i == 2 * MaxSmallBlockSize);
						data = allocate_big_block(2 * MaxSmallBlockSize);
					}
					do
					{
						--i;
						*reinterpret_cast<void**>(data) = _small_blocks[i];
						_small_blocks[i] = data;
						data = static_cast<uint8_t*>(data) + (1 << i);
					} while (i > level);
				}
			}
		}
		_buffer_memory_status._total_capacity.add(capacity);
	#if Y_BUFFER_TRACK_MAX_CAPACITY
		_buffer_memory_status._max_capacity.update(capacity);
	#endif
	#if Y_BUFFER_TRACK_ALLOCATION_COUNT
		_buffer_memory_status._allocations.add(1);
		++_buffer_memory_status._total_allocations;
	#endif
		return data;
	}

	size_t buffer_memory_capacity(size_t size) noexcept
	{
		const auto granularity_mask = buffer_memory_granularity() - 1;
		auto capacity = (size + granularity_mask) & ~granularity_mask;
		return capacity > MaxSmallBlockSize ? capacity : next_power_of_2(capacity);
	}

	void buffer_memory_deallocate(void* data, size_t capacity) noexcept
	{
		assert(data);
		assert(capacity > 0 && capacity == buffer_memory_capacity(capacity));
		if (capacity > MaxSmallBlockSize)
		{
			pages_deallocate(data, capacity);
			_buffer_memory_status._total_allocated._value.fetch_sub(capacity);
		}
		else
		{
			const auto level = level_from_capacity(capacity);
			{
				std::lock_guard<std::mutex> lock(_small_blocks_mutex);
				*reinterpret_cast<void**>(data) = _small_blocks[level];
				_small_blocks[level] = data;
			}
		}
		_buffer_memory_status._total_capacity._value.fetch_sub(capacity);
	#if Y_BUFFER_TRACK_ALLOCATION_COUNT
		_buffer_memory_status._allocations._value.fetch_sub(1);
	#endif
	}

	size_t buffer_memory_granularity() noexcept
	{
		static const auto page_size = pages_granularity();
		assert(is_power_of_2(page_size));
		return page_size;
	}

	void* buffer_memory_reallocate(void* old_data, size_t old_capacity, size_t new_capacity, size_t old_size)
	{
		assert(old_data);
		assert(old_capacity > 0 && old_capacity == buffer_memory_capacity(old_capacity));
		assert(new_capacity > 0 && new_capacity == buffer_memory_capacity(new_capacity));
		if (old_capacity > MaxSmallBlockSize && new_capacity > MaxSmallBlockSize)
		{
			const auto new_data = pages_reallocate(old_data, old_capacity, new_capacity);
			if (!new_data)
				throw std::bad_alloc();
		#if Y_BUFFER_TRACK_ALLOCATION_COUNT
			++_buffer_memory_status._total_reallocations; // TODO: Perhaps pure shrinking reallocations should be counted differently.
		#endif
			return new_data;
		}
		else
		{
			// This situation is rare, so let's do something simple.
			const auto new_data = buffer_memory_allocate(new_capacity);
			if (old_size > 0)
				::memcpy(new_data, old_data, old_size);
			buffer_memory_deallocate(old_data, old_capacity);
		#if Y_BUFFER_TRACK_ALLOCATION_COUNT
			++_buffer_memory_status._total_reallocations;
		#endif
			return new_data;
		}
	}
}
