/// \file
/// \brief

#ifndef __Y_BUFFER_HPP
#define __Y_BUFFER_HPP

#include <Yttrium/allocator.hpp>

namespace Yttrium
{

///

class Y_API Buffer
{
public:

	///

	Buffer(Allocator* allocator = HeapAllocator::instance()) noexcept
		: _data(nullptr)
		, _size(0)
		, _allocator(allocator)
	{
	}

	///

	Buffer(const Buffer &buffer, Allocator* allocator = HeapAllocator::instance()) noexcept;

	///

	Buffer(size_t size, Allocator* allocator = HeapAllocator::instance()) noexcept;

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

#endif // __Y_BUFFER_HPP
