#include <Yttrium/buffer.h>

#include <cstring> // memcmp

namespace Yttrium
{

Buffer::Buffer(const Buffer &buffer, Allocator* allocator)
	: _data(buffer._size ? allocator->allocate(buffer._size) : nullptr)
	, _size(buffer._size)
	, _allocator(allocator)
{
}

Buffer::Buffer(size_t size, Allocator* allocator)
	: _data(size ? allocator->allocate(size) : nullptr)
	, _size(size)
	, _allocator(allocator)
{
}

Buffer::~Buffer()
{
	_allocator->deallocate(_data);
}

void Buffer::resize(size_t size)
{
	if (_size < size)
	{
		_data = (_data ? _allocator->reallocate(_data, size) : _allocator->allocate(size));
		_size = size;
	}
}

bool Buffer::operator ==(const Buffer& buffer) const
{
	return (_size == buffer._size && !memcmp(_data, buffer._data, _size));
}

bool Buffer::operator !=(const Buffer& buffer) const
{
	return (_size != buffer._size || memcmp(_data, buffer._data, _size));
}

} // namespace Yttrium
