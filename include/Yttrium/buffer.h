/// \file
/// \brief

#ifndef __Y_BUFFER_H
#define __Y_BUFFER_H

#include <Yttrium/allocator.h>

namespace Yttrium
{

///

class Y_API Buffer
{
public:

	///

	Buffer(Allocator* allocator = DefaultAllocator) noexcept
		: _data(nullptr)
		, _size(0)
		, _allocator(allocator)
	{
	}

	///

	Buffer(const Buffer &buffer, Allocator* allocator = DefaultAllocator) noexcept;

	///

	Buffer(size_t size, Allocator* allocator = DefaultAllocator) noexcept;

	///

	~Buffer() noexcept;

public:

	///

	const void *const_data() const noexcept
	{
		return _data;
	}

	///

	const void *const_data(size_t offset) noexcept
	{
		return static_cast<const char *>(_data) + offset;
	}

	///

	void *data() noexcept
	{
		return _data;
	}

	///

	void *data(size_t offset) noexcept
	{
		return static_cast<char *>(_data) + offset;
	}

	///

	const void *data() const noexcept
	{
		return _data;
	}

	///

	void resize(size_t size) noexcept;

	///

	size_t size() const noexcept
	{
		return _size;
	}

private:

	void      *_data;
	size_t     _size;
	Allocator *_allocator;
};

} // namespace Yttrium

#endif // __Y_BUFFER_H
