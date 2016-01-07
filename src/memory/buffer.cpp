#include <yttrium/memory/buffer.h>

#include <yttrium/utils.h>
#include "buffer_memory.h"

#include <cstring>

namespace Yttrium
{
	Buffer::Buffer(size_t size)
		: _size(size)
		, _capacity(buffer_memory_capacity(_size))
		, _data(_capacity > 0 ? buffer_memory_allocate(_capacity) : nullptr)
	{
	#if Y_ENABLE_BUFFER_MEMORY_TRACKING
		buffer_memory_track_size_allocation(_size);
	#endif
	}

	void Buffer::reserve(size_t capacity)
	{
		if (capacity <= _capacity)
			return;
		const auto new_capacity = buffer_memory_capacity(capacity);
		const auto new_data = _data
			? buffer_memory_reallocate(_data, _capacity, new_capacity, _size)
			: buffer_memory_allocate(new_capacity);
		_capacity = new_capacity;
		_data = new_data;
	}

	void Buffer::reset(size_t size)
	{
		if (size > _capacity)
		{
			const auto new_capacity = buffer_memory_capacity(size);
			const auto new_data = _data
				? buffer_memory_reallocate(_data, _capacity, new_capacity, 0)
				: buffer_memory_allocate(new_capacity);
			_capacity = new_capacity;
			_data = new_data;
		}
	#if Y_ENABLE_BUFFER_MEMORY_TRACKING
		buffer_memory_track_size_change(_size, size);
	#endif
		_size = size;
	}

	void Buffer::resize(size_t size)
	{
		if (size > _capacity)
		{
			const auto new_capacity = buffer_memory_capacity(size);
			const auto new_data = _data
				? buffer_memory_reallocate(_data, _capacity, new_capacity, min(size, _size))
				: buffer_memory_allocate(new_capacity);
			_capacity = new_capacity;
			_data = new_data;
		}
	#if Y_ENABLE_BUFFER_MEMORY_TRACKING
		buffer_memory_track_size_change(_size, size);
	#endif
		_size = size;
	}

	void Buffer::shrink_to_fit()
	{
		const auto new_capacity = buffer_memory_capacity(_size);
		if (new_capacity < _capacity)
		{
			if (!new_capacity)
			{
				buffer_memory_deallocate(_data, _capacity);
				_data = nullptr;
			}
			else
				_data = buffer_memory_reallocate(_data, _capacity, new_capacity, _size);
			_capacity = new_capacity;
		}
	}

	size_t Buffer::max_total_size() noexcept
	{
	#if Y_ENABLE_BUFFER_MEMORY_TRACKING
		return buffer_memory_max_total_size();
	#else
		return 0;
	#endif
	}

	size_t Buffer::memory_granularity() noexcept
	{
		return buffer_memory_granularity();
	}

	size_t Buffer::total_size() noexcept
	{
	#if Y_ENABLE_BUFFER_MEMORY_TRACKING
		return buffer_memory_total_size();
	#else
		return 0;
	#endif
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
			buffer_memory_deallocate(_data, _capacity);
		#if Y_ENABLE_BUFFER_MEMORY_TRACKING
			buffer_memory_track_size_deallocation(_size);
		#endif
		}
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		if (_data)
		{
			buffer_memory_deallocate(_data, _capacity);
		#if Y_ENABLE_BUFFER_MEMORY_TRACKING
			buffer_memory_track_size_deallocation(_size);
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
