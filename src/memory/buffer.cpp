#include <yttrium/memory/buffer.h>

#include <yttrium/utils.h>
#include "buffer_memory.h"

#include <cstring>

namespace Yttrium
{
	Buffer::Buffer(size_t size)
		: _size(size)
		, _capacity(BufferMemory::capacity_for_size(_size))
		, _data(_capacity > 0 ? _buffer_memory.allocate(_capacity) : nullptr)
	{
	}

	Buffer::Buffer(size_t size, const void* data)
		: Buffer(size)
	{
		std::memcpy(_data, data, size);
	}

	void Buffer::reserve(size_t capacity)
	{
		if (capacity <= _capacity)
			return;
		const auto new_capacity = BufferMemory::capacity_for_size(capacity);
		const auto new_data = _data
			? _buffer_memory.reallocate(_data, _capacity, new_capacity, _size)
			: _buffer_memory.allocate(new_capacity);
		_capacity = new_capacity;
		_data = new_data;
	}

	void Buffer::reset(size_t size)
	{
		if (size > _capacity)
		{
			const auto new_capacity = BufferMemory::capacity_for_size(size);
			const auto new_data = _data
				? _buffer_memory.reallocate(_data, _capacity, new_capacity, 0)
				: _buffer_memory.allocate(new_capacity);
			_capacity = new_capacity;
			_data = new_data;
		}
		_size = size;
	}

	void Buffer::resize(size_t size)
	{
		if (size > _capacity)
		{
			const auto new_capacity = BufferMemory::capacity_for_size(size);
			const auto new_data = _data
				? _buffer_memory.reallocate(_data, _capacity, new_capacity, std::min(size, _size))
				: _buffer_memory.allocate(new_capacity);
			_capacity = new_capacity;
			_data = new_data;
		}
		_size = size;
	}

	void Buffer::shrink_to_fit()
	{
		const auto new_capacity = BufferMemory::capacity_for_size(_size);
		if (new_capacity < _capacity)
		{
			if (!new_capacity)
			{
				_buffer_memory.deallocate(_data, _capacity);
				_data = nullptr;
			}
			else
				_data = _buffer_memory.reallocate(_data, _capacity, new_capacity, _size);
			_capacity = new_capacity;
		}
	}

	size_t Buffer::max_total_capacity() noexcept
	{
		return BufferMemory::max_total_capacity();
	}

	size_t Buffer::memory_granularity() noexcept
	{
		return BufferMemory::granularity();
	}

	size_t Buffer::total_capacity() noexcept
	{
		return BufferMemory::total_capacity();
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
			_buffer_memory.deallocate(_data, _capacity);
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		if (_data)
			_buffer_memory.deallocate(_data, _capacity);
		_size = other._size;
		_capacity = other._capacity;
		_data = other._data;
		other._data = nullptr;
		return *this;
	}

	bool operator==(const Buffer& lhs, const Buffer& rhs) noexcept
	{
		return lhs.size() == rhs.size() && (lhs.size() == 0 || !std::memcmp(lhs.data(), rhs.data(), lhs.size()));
	}
}
