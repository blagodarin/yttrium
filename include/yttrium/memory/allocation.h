/// \file
/// \brief

#ifndef _include_yttrium_memory_allocation_h_
#define _include_yttrium_memory_allocation_h_

#include <yttrium/memory/allocator.h>

#include <cassert>

namespace Yttrium
{
	class Allocation
	{
	public:

		Allocation() = default;

		Allocation(Allocator& allocator, size_t size)
			: _allocator(&allocator)
			, _pointer(_allocator->allocate(size))
		{
		}

		Allocation(Allocation&& other) noexcept
			: _allocator(other._allocator)
			, _pointer(other._pointer)
		{
			other._pointer = nullptr;
		}

		Allocation& operator=(Allocation&& other) noexcept
		{
			if (_pointer)
				_allocator->deallocate(_pointer);
			_allocator = other._allocator;
			_pointer = other._pointer;
			other._pointer = nullptr;
			return *this;
		}

		~Allocation()
		{
			if (_pointer)
				_allocator->deallocate(_pointer);
		}

		void* get() const noexcept
		{
			return _pointer;
		}

		explicit operator bool() const noexcept
		{
			return _pointer;
		}

		Allocation(const Allocation&) = delete;
		Allocation& operator=(const Allocation&) = delete;

	private:
		Allocator* _allocator = nullptr;
		void* _pointer = nullptr;
	};
}

#endif
