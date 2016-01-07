#include "buffer_memory.h"

#include <yttrium/global.h>
#include <yttrium/utils.h>
#include "pages.h"

#include <atomic>
#include <cassert>
#include <cstring>
#include <mutex>
#include <new>

#define ENABLE_SYNCHRONIZATION 1 // Disable for profiling purposes only.

#define PRINT_TRACKING_INFO    Y_IS_DEBUG
#define TRACK_ALLOCATION_COUNT Y_IS_DEBUG
#define TRACK_MAX_ALLOCATION   Y_IS_DEBUG
#define TRACK_MAX_CAPACITY     Y_IS_DEBUG
#define TRACK_MAX_SIZE         Y_IS_DEBUG
#define TRACK_TOTAL_CAPACITY   Y_IS_DEBUG

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
	#if ENABLE_SYNCHRONIZATION
		std::mutex _small_blocks_mutex;
	#endif

		size_t level_from_capacity(size_t capacity) noexcept
		{
			assert(is_power_of_2(capacity));
			size_t level = 0;
			for (auto i = capacity; i > 1; i >>= 1)
				++level;
			assert(size_t{1} << level == capacity);
			return level;
		}

		struct BufferMemoryStatus
		{
			struct AtomicMax
			{
				std::atomic<size_t> _value{0};

				void update(size_t new_value) noexcept
				{
					auto old_value = _value.load();
					while (old_value < new_value)
						if (_value.compare_exchange_strong(old_value, new_value))
							break;
				}
			};

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

			AtomicPair _total_allocated;
		#if TRACK_TOTAL_CAPACITY
			AtomicPair _total_capacity;
		#endif
			AtomicPair _total_size;
		#if TRACK_MAX_ALLOCATION
			AtomicMax _max_allocation;
		#endif
		#if TRACK_MAX_CAPACITY
			AtomicMax _max_capacity;
		#endif
		#if TRACK_MAX_SIZE
			AtomicMax _max_size;
		#endif
		#if TRACK_ALLOCATION_COUNT
			AtomicPair _allocations;
			std::atomic<size_t> _total_system_allocations{0};
			std::atomic<size_t> _total_allocations{0};
			std::atomic<size_t> _total_reallocations{0};
		#endif

		#if PRINT_TRACKING_INFO
			~BufferMemoryStatus()
			{
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

				const auto max_total_size = _total_size._max_value.load();
				if (!max_total_size)
					return;
				std::cerr << "(DEBUG) Buffer memory statistics:";
				std::cerr << "\n(DEBUG)  * max_total_allocated : " << human_readable_size(_total_allocated._max_value);
			#if TRACK_TOTAL_CAPACITY
				std::cerr << "\n(DEBUG)  * max_total_capacity : " << human_readable_size(_total_capacity._max_value);
			#endif
				std::cerr << "\n(DEBUG)  * max_total_size : " << human_readable_size(_total_size._max_value);
			#if TRACK_MAX_ALLOCATION
				std::cerr << "\n(DEBUG)  * max_allocation : " << human_readable_size(_max_allocation._value);
			#endif
			#if TRACK_MAX_CAPACITY
				std::cerr << "\n(DEBUG)  * max_capacity : " << human_readable_size(_max_capacity._value);
			#endif
			#if TRACK_MAX_SIZE
				std::cerr << "\n(DEBUG)  * max_size : " << human_readable_size(_max_size._value);
			#endif
			#if TRACK_ALLOCATION_COUNT
				std::cerr << "\n(DEBUG)  * max_allocations : " << _allocations._max_value;
				std::cerr << "\n(DEBUG)  * total_system_allocations : " << _total_system_allocations;
				std::cerr << "\n(DEBUG)  * total_allocations : " << _total_allocations;
				std::cerr << "\n(DEBUG)  * total_reallocations : " << _total_reallocations;
			#endif
			#if Y_IS_DEBUG
				std::cerr << "\n(DEBUG)  * remaining_blocks : " << human_readable_size(_total_allocated._value);
				{
				#if ENABLE_SYNCHRONIZATION
					std::lock_guard<std::mutex> lock(_small_blocks_mutex);
				#endif
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
				const auto total_size = _total_size._value.load();
				if (total_size > 0)
				{
					std::cerr << "(ERROR) Buffer memory leaked: " << human_readable_size(total_size);
				#if TRACK_ALLOCATION_COUNT
					std::cerr << " (in " << _allocations._value << " allocations)";
				#endif
					std::cerr << std::endl;
				}
			}
		#endif

			void track_capacity_allocation(size_t capacity) noexcept
			{
				Y_UNUSED(capacity);
			#if TRACK_TOTAL_CAPACITY
				_total_capacity.add(capacity);
			#endif
			#if TRACK_MAX_CAPACITY
				_max_capacity.update(capacity);
			#endif
			#if TRACK_ALLOCATION_COUNT
				_allocations.add(1);
				++_total_allocations;
			#endif
			}

			void track_capacity_change(size_t old_capacity, size_t new_capacity) noexcept
			{
				if (new_capacity > old_capacity)
				{
				#if TRACK_TOTAL_CAPACITY
					_total_capacity.add(new_capacity - old_capacity);
				#endif
				#if TRACK_MAX_CAPACITY
					_max_capacity.update(new_capacity);
				#endif
				}
				else
					_total_size._value.fetch_sub(old_capacity - new_capacity);
			#if TRACK_ALLOCATION_COUNT
				++_total_reallocations; // TODO: Perhaps pure shrinking reallocations should be counted differently.
			#endif
			}

			void track_capacity_deallocation(size_t capacity) noexcept
			{
				Y_UNUSED(capacity);
			#if TRACK_TOTAL_CAPACITY
				_total_capacity._value.fetch_sub(capacity);
			#endif
			#if TRACK_ALLOCATION_COUNT
				_allocations._value.fetch_sub(1);
			#endif
			}

			void track_size_allocation(size_t size) noexcept
			{
				_total_size.add(size);
			#if TRACK_MAX_SIZE
				_max_size.update(size);
			#endif
			}

			void track_size_change(size_t old_size, size_t new_size) noexcept
			{
				if (new_size > old_size)
				{
					_total_size.add(new_size - old_size);
				#if TRACK_MAX_SIZE
					_max_size.update(new_size);
				#endif
				}
				else
					_total_size._value.fetch_sub(old_size - new_size);
			}

			void track_size_deallocation(size_t size) noexcept
			{
				_total_size._value.fetch_sub(size);
			}
		};

		BufferMemoryStatus _buffer_memory_status;

		void* allocate_big_block(size_t size)
		{
			const auto data = pages_allocate(size);
			if (!data)
				throw std::bad_alloc();
			_buffer_memory_status._total_allocated.add(size);
		#if TRACK_MAX_ALLOCATION
			_buffer_memory_status._max_allocation.update(size);
		#endif
		#if TRACK_ALLOCATION_COUNT
			++_buffer_memory_status._total_system_allocations;
		#endif
			return data;
		}
	}

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
			#if ENABLE_SYNCHRONIZATION
				std::lock_guard<std::mutex> lock(_small_blocks_mutex);
			#endif
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
		_buffer_memory_status.track_capacity_allocation(capacity);
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
			#if ENABLE_SYNCHRONIZATION
				std::lock_guard<std::mutex> lock(_small_blocks_mutex);
			#endif
				*reinterpret_cast<void**>(data) = _small_blocks[level];
				_small_blocks[level] = data;
			}
		}
		_buffer_memory_status.track_capacity_deallocation(capacity);
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
		assert(old_capacity != new_capacity);
		if (old_capacity > MaxSmallBlockSize && new_capacity > MaxSmallBlockSize)
		{
			const auto new_data = pages_reallocate(old_data, old_capacity, new_capacity);
			if (!new_data)
				throw std::bad_alloc();
			_buffer_memory_status.track_capacity_change(old_capacity, new_capacity);
		#if TRACK_MAX_ALLOCATION
			_buffer_memory_status._max_allocation.update(new_capacity);
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
		#if TRACK_ALLOCATION_COUNT
			++_buffer_memory_status._total_reallocations;
		#endif
			return new_data;
		}
	}

#if Y_ENABLE_BUFFER_MEMORY_TRACKING
	size_t buffer_memory_max_total_size() noexcept
	{
		return _buffer_memory_status._total_size._max_value;
	}

	size_t buffer_memory_total_size() noexcept
	{
		return _buffer_memory_status._total_size._value;
	}

	void buffer_memory_track_size_allocation(size_t size) noexcept
	{
		_buffer_memory_status.track_size_allocation(size);
	}

	void buffer_memory_track_size_change(size_t old_size, size_t new_size) noexcept
	{
		_buffer_memory_status.track_size_change(old_size, new_size);
	}

	void buffer_memory_track_size_deallocation(size_t size) noexcept
	{
		_buffer_memory_status.track_size_deallocation(size);
	}
#endif
}
