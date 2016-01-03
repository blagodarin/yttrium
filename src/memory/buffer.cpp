#include <yttrium/memory/buffer.h>

#include "pages.h"

#include <atomic>
#include <cstring>
#include <new>

#if Y_IS_DEBUG
	#include <iostream>
#endif

// TODO: Accelerate small buffers (see benchmark).

#define TRACK_WASTED_MEMORY Y_IS_DEBUG

namespace Yttrium
{
	namespace
	{
		struct AtomicPair
		{
			std::atomic<size_t> _value{0};
			std::atomic<size_t> _max_value{0};

			void add(size_t difference)
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

		#if Y_IS_DEBUG
			~BufferMemoryStatus()
			{
				const auto max_allocated = _allocated._max_value.load();
				std::cerr << "(DEBUG) Max buffer memory allocated: " << max_allocated / 1024 << " KiB" << std::endl;
			#if TRACK_WASTED_MEMORY
				const auto max_wasted = _wasted._max_value.load();
				std::cerr << "(DEBUG) Max buffer memory wasted: " << max_wasted << " B" << std::endl;
			#endif
				const auto allocated = _allocated._value.load();
				if (allocated > 0)
					std::cerr << "(ERROR) Buffer memory leaked: " << allocated / 1024 << " KiB" << std::endl;
			}
		#endif
		};

		BufferMemoryStatus _buffer_memory_status;
	}

	Buffer::Buffer(size_t size)
		: _size(size)
		, _capacity(pages_size(_size))
		, _data(pages_allocate(_capacity))
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
			_data = pages_reallocate(_data, _capacity, new_capacity);
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
			const auto new_data = _data ? pages_reallocate(_data, _capacity, new_capacity) : pages_allocate(new_capacity);
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
			pages_deallocate(_data, _capacity);
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
			pages_deallocate(_data, _capacity);
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
