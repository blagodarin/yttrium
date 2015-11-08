#include <yttrium/memory/pages.h>

#include "pages.h"

#include <new>

namespace Yttrium
{
	PageAllocation::PageAllocation(size_t min_size)
		: _size(pages_size(min_size))
		, _data(pages_allocate(_size))
	{
		if (!_data)
			throw std::bad_alloc();
	}

	PageAllocation::PageAllocation(PageAllocation&& allocation) noexcept
		: _size(allocation._size)
		, _data(allocation._data)
	{
		allocation._data = nullptr;
	}

	PageAllocation::~PageAllocation() noexcept
	{
		if (_data)
			pages_deallocate(_data, _size);
	}

	PageAllocation& PageAllocation::operator=(PageAllocation&& allocation) noexcept
	{
		if (_data)
			pages_deallocate(_data, _size);
		_size = allocation._size;
		_data = allocation._data;
		allocation._data = nullptr;
		return *this;
	}
}
