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
	#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
		if (_capacity > _size)
			_buffer_memory_status._total_capacity_wasted.add(_capacity - _size);
	#endif
	}

	void Buffer::reset(size_t size)
	{
		if (size > _capacity)
		{
			const auto new_capacity = buffer_memory_capacity(size);
			const auto new_data = _data
				? buffer_memory_reallocate(_data, _capacity, new_capacity, 0)
				: buffer_memory_allocate(new_capacity);
		#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
			_buffer_memory_status._total_capacity_wasted._value.fetch_sub(_capacity - _size);
			_buffer_memory_status._total_capacity_wasted.add(new_capacity - size);
		#endif
			_capacity = new_capacity;
			_data = new_data;
		}
	#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
		else if (size < _size)
			_buffer_memory_status._total_capacity_wasted.add(_size - size);
		else
			_buffer_memory_status._total_capacity_wasted._value.fetch_sub(size - _size);
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
		#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
			_buffer_memory_status._total_capacity_wasted._value.fetch_sub(_capacity - _size);
			_buffer_memory_status._total_capacity_wasted.add(new_capacity - size);
		#endif
			_capacity = new_capacity;
			_data = new_data;
		}
	#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
		else if (size < _size)
			_buffer_memory_status._total_capacity_wasted.add(_size - size);
		else
			_buffer_memory_status._total_capacity_wasted._value.fetch_sub(size - _size);
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
			#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
				_buffer_memory_status._total_capacity_wasted._value.fetch_sub(_capacity);
			#endif
				_data = nullptr;
			}
			else
			{
				_data = buffer_memory_reallocate(_data, _capacity, new_capacity, _size);
			#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
				_buffer_memory_status._total_capacity_wasted._value.fetch_sub(_capacity - new_capacity);
			#endif
			}
			_capacity = new_capacity;
		}
	}

	size_t Buffer::max_total_capacity() noexcept
	{
		return _buffer_memory_status._total_capacity._max_value;
	}

	size_t Buffer::memory_granularity() noexcept
	{
		return buffer_memory_granularity();
	}

	size_t Buffer::total_capacity() noexcept
	{
		return _buffer_memory_status._total_capacity._value;
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
		#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
			_buffer_memory_status._total_capacity_wasted._value.fetch_sub(_capacity - _size);
		#endif
		}
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		if (_data)
		{
			buffer_memory_deallocate(_data, _capacity);
		#if Y_BUFFER_TRACK_TOTAL_CAPACITY_WASTED
			_buffer_memory_status._total_capacity_wasted._value.fetch_sub(_capacity - _size);
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
