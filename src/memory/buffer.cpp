#include <yttrium/memory/buffer.h>

#include <yttrium/utils.h>
#include "pages.h"

#include <atomic>
#include <cassert>
#include <cstring>
#include <new>

#define PRINT_TRACKING_INFO Y_IS_DEBUG
#define TRACK_WASTED_MEMORY Y_IS_DEBUG

#if PRINT_TRACKING_INFO
	#include <iostream>
	#include <sstream>
#endif

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
				std::cerr << "(DEBUG) Max buffer memory allocated: " << human_readable_size(max_allocated) << std::endl;
			#if TRACK_WASTED_MEMORY
				const auto max_wasted = _wasted._max_value.load();
				std::cerr << "(DEBUG) Max buffer memory wasted: " << human_readable_size(max_wasted) << std::endl;
			#endif
				const auto allocated = _allocated._value.load();
				if (allocated > 0)
					std::cerr << "(ERROR) Buffer memory leaked: " << human_readable_size(allocated) << std::endl;
			}
		#endif
		};

		BufferMemoryStatus _buffer_memory_status;

		void* buffer_allocate(size_t size) noexcept
		{
			assert(size > 0 && size == pages_size(size));
			if (size >= SmallBufferThreshold)
				return pages_allocate(size);
			const auto granularity = Buffer::memory_granularity();
			const auto buffer = ::malloc(size + granularity);
			if (!buffer)
				return nullptr;
			auto memory_offset = reinterpret_cast<uintptr_t>(buffer) + sizeof(void*);
			const auto granularity_mask = uintptr_t{granularity - 1};
			memory_offset = (memory_offset + granularity_mask) & ~granularity_mask;
			const auto result = reinterpret_cast<void*>(memory_offset);
			*(static_cast<void**>(result) - 1) = buffer;
			_buffer_memory_status._allocated.add(granularity);
		#if TRACK_WASTED_MEMORY
			_buffer_memory_status._wasted.add(granularity);
		#endif
			return result;
		}

		void buffer_deallocate(void* pointer, size_t size) noexcept
		{
			assert(pointer);
			assert(size > 0 && size == pages_size(size));
			if (size >= SmallBufferThreshold)
				return pages_deallocate(pointer, size);
			::free(*(static_cast<void**>(pointer) - 1));
			const auto granularity = Buffer::memory_granularity();
		#if TRACK_WASTED_MEMORY
			_buffer_memory_status._wasted._value.fetch_sub(granularity);
		#endif
			_buffer_memory_status._allocated._value.fetch_sub(granularity);
		}

		void* buffer_reallocate(void* old_pointer, size_t old_size, size_t new_size) noexcept
		{
			assert(old_pointer);
			assert(old_size > 0 && old_size == pages_size(old_size));
			assert(new_size > 0 && new_size == pages_size(new_size));
			if (old_size >= SmallBufferThreshold && new_size >= SmallBufferThreshold)
				return pages_reallocate(old_pointer, old_size, new_size);
			// This is a rare situation, so let's do something simple.
			const auto new_pointer = buffer_allocate(new_size);
			::memcpy(new_pointer, old_pointer, min(old_size, new_size));
			buffer_deallocate(old_pointer, old_size);
			return new_pointer;
		}
	}

	Buffer::Buffer(size_t size)
		: _size(size)
		, _capacity(pages_size(_size))
		, _data(buffer_allocate(_capacity))
	{
		if (!_data)
			throw std::bad_alloc();
		_buffer_memory_status._allocated.add(_capacity);
	#if TRACK_WASTED_MEMORY
		if (_capacity > _size)
			_buffer_memory_status._wasted.add(_capacity - _size);
	#endif
	}

	void Buffer::shrink_to_fit() noexcept
	{
		const auto new_capacity = pages_size(_size);
		if (new_capacity < _capacity)
		{
			_data = buffer_reallocate(_data, _capacity, new_capacity);
		#if TRACK_WASTED_MEMORY
			_buffer_memory_status._wasted._value.fetch_sub(_capacity - new_capacity);
		#endif
			_buffer_memory_status._allocated._value.fetch_sub(_capacity - new_capacity);
			_capacity = new_capacity;
		}
	}

	void Buffer::reset(size_t size)
	{
		if (size > _capacity)
		{
			const auto new_capacity = pages_size(size);
			const auto new_data = _data ? buffer_reallocate(_data, _capacity, new_capacity) : buffer_allocate(new_capacity);
			if (!new_data)
				throw std::bad_alloc();
		#if TRACK_WASTED_MEMORY
			_buffer_memory_status._wasted._value.fetch_sub(_capacity - _size);
		#endif
			_buffer_memory_status._allocated.add(new_capacity - _capacity);
		#if TRACK_WASTED_MEMORY
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

	size_t Buffer::max_total_memory_allocated() noexcept
	{
		return _buffer_memory_status._allocated._max_value;
	}

	size_t Buffer::memory_granularity() noexcept
	{
		return pages_size(1);
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
			_buffer_memory_status._allocated._value.fetch_sub(_capacity);
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
			_buffer_memory_status._allocated._value.fetch_sub(_capacity);
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
