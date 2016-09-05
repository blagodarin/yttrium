/// \file
/// \brief

#ifndef _include_yttrium_memory_allocation_h_
#define _include_yttrium_memory_allocation_h_

#include <yttrium/memory/allocator.h>
#include <yttrium/memory/global.h>

#include <type_traits>

namespace Yttrium
{
	template <typename T>
	class Allocation
	{
	public:
		Allocation() = default;
		Allocation(const Allocation&) = delete;
		Allocation& operator=(const Allocation&) = delete;

		Allocation(Allocator& allocator, size_t size = sizeof(T))
			: _allocator(&allocator)
			, _pointer(static_cast<T*>(_allocator->allocate(size)))
		{
		}

		Allocation(Allocation&& other) noexcept
			: _allocator(other._allocator)
			, _pointer(other._pointer)
		{
			other._pointer = nullptr;
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		Allocation(Allocation<U>&& other) noexcept
			: _allocator(other._allocator)
			, _pointer(other._pointer)
		{
			other._pointer = nullptr;
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		Allocation(Allocator& allocator, U* pointer) noexcept
			: _allocator(&allocator)
			, _pointer(pointer)
		{
		}

		Allocation& operator=(Allocation&& other) noexcept
		{
			_allocator->deallocate(_pointer);
			_allocator = other._allocator;
			_pointer = other._pointer;
			other._pointer = nullptr;
			return *this;
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		Allocation& operator=(Allocation<U>&& other) noexcept
		{
			_allocator->deallocate(_pointer);
			_allocator = other._allocator;
			_pointer = other._pointer;
			other._pointer = nullptr;
			return *this;
		}

		~Allocation()
		{
			_allocator->deallocate(_pointer);
		}

		Allocator& allocator() const noexcept
		{
			return *_allocator;
		}

		T* get() const noexcept
		{
			return _pointer;
		}

		T* release() noexcept
		{
			auto result = _pointer;
			_pointer = nullptr;
			return result;
		}

		explicit operator bool() const noexcept
		{
			return _pointer;
		}

	private:
		Allocator* _allocator = &NoAllocator;
		T* _pointer = nullptr;
		template <typename> friend class Allocation;
	};
}

#endif