// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/memory/buffer.h>

#include "buffer_memory.h"

#include <cassert>
#include <cstring>
#include <new>

namespace Yt
{
	Buffer::Buffer(size_t size)
		: _size{ size }
		, _capacity{ BufferMemory::capacity_for_size(_size) }
		, _data{ _capacity > 0 ? _buffer_memory.allocate(_capacity) : nullptr }
	{
		if (!_data && _capacity > 0)
			throw std::bad_alloc{};
	}

	Buffer::Buffer(size_t size, const void* data)
		: Buffer{ size }
	{
		if (_data)
			std::memcpy(_data, data, size);
	}

	void Buffer::reserve(size_t capacity)
	{
		if (!try_grow(capacity, _size, false))
			throw std::bad_alloc{};
	}

	void Buffer::reset(size_t size)
	{
		if (!try_grow(size, 0, true))
			throw std::bad_alloc{};
	}

	void Buffer::resize(size_t size)
	{
		if (!try_grow(size, size < _size ? size : _size, true))
			throw std::bad_alloc{};
	}

	bool Buffer::try_grow(size_t allocate_bytes, size_t copy_bytes, bool do_resize) noexcept
	{
		if (allocate_bytes > _capacity)
		{
			assert(allocate_bytes >= copy_bytes);
			const auto new_capacity = BufferMemory::capacity_for_size(allocate_bytes);
			const auto new_data = _data
				? _buffer_memory.reallocate(_data, _capacity, new_capacity, copy_bytes)
				: _buffer_memory.allocate(new_capacity);
			if (!new_data)
				return false;
			_capacity = new_capacity;
			_data = new_data;
		}
		if (do_resize)
			_size = allocate_bytes;
		return true;
	}

	size_t Buffer::memory_granularity() noexcept
	{
		return BufferMemory::granularity();
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

	bool operator==(const Buffer& a, const Buffer& b) noexcept
	{
		return a.size() == b.size() && (a.size() == 0 || !std::memcmp(a.data(), b.data(), a.size()));
	}
}
