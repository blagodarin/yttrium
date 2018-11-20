//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/memory/buffer.h>

#include "buffer_memory.h"

#include <algorithm>
#include <cstring>

namespace Yttrium
{
	Buffer::Buffer(size_t size)
		: _size{ size }
		, _capacity{ BufferMemory::capacity_for_size(_size) }
		, _data{ _capacity > 0 ? _buffer_memory.allocate(_capacity) : nullptr }
	{
	}

	Buffer::Buffer(size_t size, const void* data)
		: Buffer{ size }
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
