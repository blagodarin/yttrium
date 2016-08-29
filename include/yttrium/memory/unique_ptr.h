/// \file
/// \brief Unique pointer.

#ifndef _include_yttrium_memory_unique_ptr_h_
#define _include_yttrium_memory_unique_ptr_h_

#include <yttrium/memory/allocation.h>

#include <utility>

namespace Yttrium
{
	template <typename> class SharedPtr;

	template <typename T>
	class UniquePtr
	{
	public:
		UniquePtr() = default;
		UniquePtr(const UniquePtr&) = delete;
		UniquePtr& operator=(const UniquePtr&) = delete;

		UniquePtr(UniquePtr&& other) noexcept
			: _allocation(std::move(other._allocation))
		{
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		UniquePtr(UniquePtr<U>&& other) noexcept
			: _allocation(std::move(other._allocation))
		{
		}

		~UniquePtr()
		{
			if (_allocation)
				_allocation.get()->~T();
		}

		UniquePtr& operator=(UniquePtr&& other) noexcept
		{
			if (_allocation)
				_allocation.get()->~T();
			_allocation = std::move(other._allocation);
			return *this;
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		UniquePtr& operator=(UniquePtr<U>&& other) noexcept
		{
			if (_allocation)
				_allocation.get()->~T();
			_allocation = std::move(other._allocation);
			return *this;
		}

		explicit operator bool() const noexcept { return static_cast<bool>(_allocation); }

		Allocator& allocator() const { return _allocation.allocator(); }
		T* get() const noexcept { return _allocation.get(); }
		T* operator->() const noexcept { return _allocation.get(); }
		T& operator*() const noexcept { return *_allocation.get(); }

	private:
		Allocation<T> _allocation;
		UniquePtr(Allocation<T>&& allocation) noexcept : _allocation(std::move(allocation)) {}
		template <typename U, typename... Args> friend UniquePtr<U> make_unique(Allocator&, Args&&...);
		template <typename> friend class UniquePtr;
		template <typename> friend class SharedPtr;
	};

	template <typename T, typename... Args>
	UniquePtr<T> make_unique(Allocator& allocator, Args&&... args)
	{
		Allocation<T> allocation(allocator);
		new(allocation.get()) T(std::forward<Args>(args)...);
		return UniquePtr<T>(std::move(allocation));
	}

	template <typename A, typename B>
	bool operator==(const UniquePtr<A>& a, const UniquePtr<B>& b) noexcept { return a.get() == b.get(); }

	template <typename A, typename B>
	bool operator!=(const UniquePtr<A>& a, const UniquePtr<B>& b) noexcept { return !(a == b); }
}

#endif
