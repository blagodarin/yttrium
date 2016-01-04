#include <yttrium/memory/buffer.h>

#include <yttrium/utils.h>
#include "pages.h"

#include <atomic>
#include <cassert>
#include <cstring>
#include <new>

#define PRINT_TRACKING_INFO Y_IS_DEBUG
#define TRACK_ALLOCATION_COUNT Y_IS_DEBUG
#define TRACK_WASTED_MEMORY Y_IS_DEBUG

#if PRINT_TRACKING_INFO
	#include <iostream>
	#include <sstream>
#endif

// TODO: Improve allocation algorithm (e.g. use buddy allocator for blocks up to 1 MiB).

namespace Yttrium
{
	namespace
	{
		const size_t SmallBufferThreshold = 512 * 1024;

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

		struct BufferMemoryStatus
		{
			AtomicPair _allocated;
		#if TRACK_ALLOCATION_COUNT
			AtomicPair _allocations;
			std::atomic<size_t> _total_allocations{0};
		#endif
		#if TRACK_WASTED_MEMORY
			AtomicPair _wasted;
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

				const auto max_allocated = _allocated._max_value.load();
				if (!max_allocated)
					return;
				std::cerr << "(DEBUG) Buffer memory statistics:";
				std::cerr << "\n(DEBUG)  * max_allocated : " << human_readable_size(_allocated._max_value);
			#if TRACK_ALLOCATION_COUNT
				std::cerr << "\n(DEBUG)  * max_allocations : " << _allocations._max_value;
				std::cerr << "\n(DEBUG)  * total_allocations : " << _total_allocations;
			#endif
			#if TRACK_WASTED_MEMORY
				std::cerr << "\n(DEBUG)  * max_wasted : " << human_readable_size(_wasted._max_value);
			#endif
				std::cerr << std::endl;
				const auto allocated = _allocated._value.load();
				if (allocated > 0)
				{
					std::cerr << "(ERROR) Buffer memory leaked: " << human_readable_size(allocated);
				#if TRACK_ALLOCATION_COUNT
					std::cerr << " (in " << _allocations._value << " allocations)";
				#endif
					std::cerr << std::endl;
				}
			}
		#endif
		};

		BufferMemoryStatus _buffer_memory_status;

		size_t buffer_granularity() noexcept
		{
			static const auto page_size = pages_granularity();
			assert(is_power_of_2(page_size));
			return page_size;
		}

		size_t buffer_capacity(size_t size) noexcept
		{
			const auto granularity_mask = buffer_granularity() - 1;
			return (size + granularity_mask) & ~granularity_mask;
		}

		void* buffer_allocate(size_t capacity)
		{
			assert(capacity > 0 && capacity == buffer_capacity(capacity));
			if (capacity >= SmallBufferThreshold)
			{
				const auto data = pages_allocate(capacity);
				if (!data)
					throw std::bad_alloc();
				_buffer_memory_status._allocated.add(capacity);
			#if TRACK_ALLOCATION_COUNT
				_buffer_memory_status._allocations.add(1);
				++_buffer_memory_status._total_allocations;
			#endif
				return data;
			}
			else
			{
				const auto granularity = buffer_granularity();
				const auto buffer = ::malloc(capacity + granularity);
				if (!buffer)
					throw std::bad_alloc();
				auto memory_offset = reinterpret_cast<uintptr_t>(buffer) + sizeof(void*);
				const auto granularity_mask = uintptr_t{granularity - 1};
				memory_offset = (memory_offset + granularity_mask) & ~granularity_mask;
				const auto data = reinterpret_cast<void*>(memory_offset);
				*(static_cast<void**>(data) - 1) = buffer;
				_buffer_memory_status._allocated.add(capacity + granularity);
			#if TRACK_ALLOCATION_COUNT
				_buffer_memory_status._allocations.add(1);
				++_buffer_memory_status._total_allocations;
			#endif
			#if TRACK_WASTED_MEMORY
				_buffer_memory_status._wasted.add(granularity);
			#endif
				return data;
			}
		}

		void buffer_deallocate(void* data, size_t capacity) noexcept
		{
			assert(data);
			assert(capacity > 0 && capacity == buffer_capacity(capacity));
			if (capacity >= SmallBufferThreshold)
			{
				pages_deallocate(data, capacity);
				_buffer_memory_status._allocated._value.fetch_sub(capacity);
			#if TRACK_ALLOCATION_COUNT
				_buffer_memory_status._allocations._value.fetch_sub(1);
			#endif
			}
			else
			{
				::free(*(static_cast<void**>(data) - 1));
				const auto granularity = buffer_granularity();
				_buffer_memory_status._allocated._value.fetch_sub(capacity + granularity);
			#if TRACK_ALLOCATION_COUNT
				_buffer_memory_status._allocations._value.fetch_sub(1);
			#endif
			#if TRACK_WASTED_MEMORY
				_buffer_memory_status._wasted._value.fetch_sub(granularity);
			#endif
			}
		}

		void* buffer_reallocate(void* old_data, size_t old_capacity, size_t new_capacity)
		{
			assert(old_data);
			assert(old_capacity > 0 && old_capacity == buffer_capacity(old_capacity));
			assert(new_capacity > 0 && new_capacity == buffer_capacity(new_capacity));
			if (old_capacity >= SmallBufferThreshold && new_capacity >= SmallBufferThreshold)
			{
				const auto new_data = pages_reallocate(old_data, old_capacity, new_capacity);
				if (!new_data)
					throw std::bad_alloc();
				return new_data;
			}
			else
			{
				// This is a rare situation, so let's do something simple.
				const auto new_data = buffer_allocate(new_capacity);
				buffer_deallocate(old_data, old_capacity);
				return new_data;
			}
		}
	}

	Buffer::Buffer(size_t size)
		: _size(size)
		, _capacity(buffer_capacity(_size))
		, _data(_capacity > 0 ? buffer_allocate(_capacity) : nullptr)
	{
	#if TRACK_WASTED_MEMORY
		if (_capacity > _size)
			_buffer_memory_status._wasted.add(_capacity - _size);
	#endif
	}

	void Buffer::reset(size_t size)
	{
		if (size > _capacity)
		{
			const auto new_capacity = buffer_capacity(size);
			const auto new_data = _data ? buffer_reallocate(_data, _capacity, new_capacity) : buffer_allocate(new_capacity);
		#if TRACK_WASTED_MEMORY
			_buffer_memory_status._wasted._value.fetch_sub(_capacity - _size);
			_buffer_memory_status._wasted.add(new_capacity - size);
		#endif
			_capacity = new_capacity;
			_data = new_data;
		}
	#if TRACK_WASTED_MEMORY
		else if (size < _size)
			_buffer_memory_status._wasted.add(_size - size);
		else
			_buffer_memory_status._wasted._value.fetch_sub(size - _size);
	#endif
		_size = size;
	}

	void Buffer::shrink_to_fit() noexcept
	{
		const auto new_capacity = buffer_capacity(_size);
		if (new_capacity < _capacity)
		{
			if (!new_capacity)
			{
				buffer_deallocate(_data, _capacity);
			#if TRACK_WASTED_MEMORY
				_buffer_memory_status._wasted._value.fetch_sub(_capacity);
			#endif
				_data = nullptr;
			}
			else
			{
				_data = buffer_reallocate(_data, _capacity, new_capacity);
			#if TRACK_WASTED_MEMORY
				_buffer_memory_status._wasted._value.fetch_sub(_capacity - new_capacity);
			#endif
			}
			_capacity = new_capacity;
		}
	}

	size_t Buffer::max_total_memory_allocated() noexcept
	{
		return _buffer_memory_status._allocated._max_value;
	}

	size_t Buffer::memory_granularity() noexcept
	{
		return buffer_granularity();
	}

	size_t Buffer::total_memory_allocated() noexcept
	{
		return _buffer_memory_status._allocated._value;
	}

	Buffer::Buffer(Buffer&& other) noexcept
		: _size(other._size)
		, _capacity(other._capacity)
		, _data(other._data)
	{
		other._data = nullptr;
	}

	Buffer::~Buffer() noexcept
	{
		if (_data)
		{
			buffer_deallocate(_data, _capacity);
		#if TRACK_WASTED_MEMORY
			_buffer_memory_status._wasted._value.fetch_sub(_capacity - _size);
		#endif
		}
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		if (_data)
		{
			buffer_deallocate(_data, _capacity);
		#if TRACK_WASTED_MEMORY
			_buffer_memory_status._wasted._value.fetch_sub(_capacity - _size);
		#endif
		}
		_size = other._size;
		_capacity = other._capacity;
		_data = other._data;
		other._data = nullptr;
		return *this;
	}

	bool operator==(const Buffer& lhs, const Buffer& rhs) noexcept
	{
		return lhs.size() == rhs.size() && (lhs.size() == 0 || !::memcmp(lhs.data(), rhs.data(), lhs.size()));
	}
}
