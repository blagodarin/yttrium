#include "buffer_memory.h"

#include <yttrium/utils.h>
#include "pages.h"

#include <cassert>
#include <cstring>
#include <new>

#define PRINT_TRACKING_INFO Y_IS_DEBUG

#if PRINT_TRACKING_INFO
	#include <iostream>
	#include <sstream>
#endif

// TODO: Improve allocation algorithm (e.g. use buddy allocator for blocks up to 1 MiB).

namespace Yttrium
{
	const size_t SmallBufferThreshold = 512 * 1024;

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

		const auto max_allocated = _allocated._max_value.load();
		if (!max_allocated)
			return;
		std::cerr << "(DEBUG) Buffer memory statistics:";
		std::cerr << "\n(DEBUG)  * max_allocated : " << human_readable_size(_allocated._max_value);
	#if Y_BUFFER_TRACK_ALLOCATION_COUNT
		std::cerr << "\n(DEBUG)  * max_allocations : " << _allocations._max_value;
		std::cerr << "\n(DEBUG)  * total_allocations : " << _total_allocations;
		std::cerr << "\n(DEBUG)  * total_reallocations : " << _total_reallocations;
	#endif
	#if Y_BUFFER_TRACK_WASTED_MEMORY
		std::cerr << "\n(DEBUG)  * max_wasted : " << human_readable_size(_wasted._max_value);
	#endif
		std::cerr << std::endl;
		const auto allocated = _allocated._value.load();
		if (allocated > 0)
		{
			std::cerr << "(ERROR) Buffer memory leaked: " << human_readable_size(allocated);
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
		if (capacity >= SmallBufferThreshold)
		{
			const auto data = pages_allocate(capacity);
			if (!data)
				throw std::bad_alloc();
			_buffer_memory_status._allocated.add(capacity);
		#if Y_BUFFER_TRACK_ALLOCATION_COUNT
			_buffer_memory_status._allocations.add(1);
			++_buffer_memory_status._total_allocations;
		#endif
			return data;
		}
		else
		{
			const auto granularity = buffer_memory_granularity();
			const auto buffer = ::malloc(capacity + granularity);
			if (!buffer)
				throw std::bad_alloc();
			auto memory_offset = reinterpret_cast<uintptr_t>(buffer) + sizeof(void*);
			const auto granularity_mask = uintptr_t{granularity - 1};
			memory_offset = (memory_offset + granularity_mask) & ~granularity_mask;
			const auto data = reinterpret_cast<void*>(memory_offset);
			*(static_cast<void**>(data) - 1) = buffer;
			_buffer_memory_status._allocated.add(capacity + granularity);
		#if Y_BUFFER_TRACK_ALLOCATION_COUNT
			_buffer_memory_status._allocations.add(1);
			++_buffer_memory_status._total_allocations;
		#endif
		#if Y_BUFFER_TRACK_WASTED_MEMORY
			_buffer_memory_status._wasted.add(granularity);
		#endif
			return data;
		}
	}

	size_t buffer_memory_capacity(size_t size) noexcept
	{
		const auto granularity_mask = buffer_memory_granularity() - 1;
		return (size + granularity_mask) & ~granularity_mask;
	}

	void buffer_memory_deallocate(void* data, size_t capacity) noexcept
	{
		assert(data);
		assert(capacity > 0 && capacity == buffer_memory_capacity(capacity));
		if (capacity >= SmallBufferThreshold)
		{
			pages_deallocate(data, capacity);
			_buffer_memory_status._allocated._value.fetch_sub(capacity);
		#if Y_BUFFER_TRACK_ALLOCATION_COUNT
			_buffer_memory_status._allocations._value.fetch_sub(1);
		#endif
		}
		else
		{
			::free(*(static_cast<void**>(data) - 1));
			const auto granularity = buffer_memory_granularity();
			_buffer_memory_status._allocated._value.fetch_sub(capacity + granularity);
		#if Y_BUFFER_TRACK_ALLOCATION_COUNT
			_buffer_memory_status._allocations._value.fetch_sub(1);
		#endif
		#if Y_BUFFER_TRACK_WASTED_MEMORY
			_buffer_memory_status._wasted._value.fetch_sub(granularity);
		#endif
		}
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
		if (old_capacity >= SmallBufferThreshold && new_capacity >= SmallBufferThreshold)
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
