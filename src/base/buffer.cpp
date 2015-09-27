#include <yttrium/buffer.h>

#include <yttrium/allocator.h>

#include <cstring>

namespace Yttrium
{
	Buffer::Buffer(const Buffer& buffer, Allocator* allocator)
		: _allocator(allocator ? allocator : buffer._allocator)
		, _data(buffer._size > 0 ? _allocator->allocate(buffer._size) : nullptr)
		, _size(buffer._size)
	{
	}

	Buffer::Buffer(size_t size, Allocator* allocator)
		: _allocator(allocator)
		, _data(size > 0 ? allocator->allocate(size) : nullptr)
		, _size(size)
	{
	}

	Buffer::~Buffer()
	{
		_allocator->deallocate(_data);
	}

	void Buffer::resize(size_t size)
	{
		if (size <= _size)
			return;
		const auto new_data = _allocator->allocate(size);
		if (_data)
		{
			::memcpy(new_data, _data, _size);
			_allocator->deallocate(_data, true);
		}
		_data = new_data;
		_size = size;
	}

	void Buffer::swap(Buffer& buffer)
	{
		const auto data = _data;
		const auto size = _size;
		const auto allocator = _allocator;

		_data = buffer._data;
		_size = buffer._size;
		_allocator = buffer._allocator;

		buffer._data = data;
		buffer._size = size;
		buffer._allocator = allocator;
	}

	void Buffer::swap(Buffer&& buffer)
	{
		const auto data = _data;
		const auto size = _size;
		const auto allocator = _allocator;

		_data = buffer._data;
		_size = buffer._size;
		_allocator = buffer._allocator;

		buffer._data = data;
		buffer._size = size;
		buffer._allocator = allocator;
	}

	bool Buffer::operator==(const Buffer& buffer) const
	{
		return _size == buffer._size && !::memcmp(_data, buffer._data, _size);
	}

	bool Buffer::operator!=(const Buffer& buffer) const
	{
		return _size != buffer._size || ::memcmp(_data, buffer._data, _size);
	}
}
