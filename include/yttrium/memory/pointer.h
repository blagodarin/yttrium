/// \file
/// \brief Unique pointer.

#ifndef _include_yttrium_memory_pointer_h_
#define _include_yttrium_memory_pointer_h_

#include <yttrium/memory/allocation.h>

#include <utility>
#include <type_traits>

namespace Yttrium
{
	/// Unique pointer.
	template <typename T>
	class Pointer
	{
		template <typename> friend class Pointer;
	public:

		Pointer() = default;

		Pointer(const Pointer&) = delete;

		Pointer(Pointer&& p) noexcept
			: _allocation(std::move(p._allocation))
		{
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		Pointer(Pointer<U>&& p) noexcept
			: _allocation(std::move(p._allocation))
		{
		}

		Pointer(Allocation&& allocation) noexcept
			: _allocation(std::move(allocation))
		{
		}

		~Pointer()
		{
			if (_allocation)
				get()->~T();
		}

		Pointer& operator=(const Pointer&) = delete;

		Pointer& operator=(Pointer&& p)
		{
			if (_allocation)
				get()->~T();
			_allocation = std::move(p._allocation);
			return *this;
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		Pointer& operator=(Pointer<U>&& p)
		{
			if (_allocation)
				get()->~T();
			_allocation = std::move(p._allocation);
			return *this;
		}

		explicit operator bool() const noexcept { return static_cast<bool>(_allocation); }

		T* get() const noexcept { return static_cast<T*>(_allocation.get()); }

		T* operator->() const noexcept { return get(); }

		T& operator*() const noexcept { return *get(); }

	private:
		Allocation _allocation;
	};

	///
	template <typename T, typename... Args>
	Pointer<T> make_pointer(Allocator& allocator, Args&&... args)
	{
		Allocation allocation(allocator, sizeof(T));
		new(allocation.get()) T(std::forward<Args>(args)...);
		return Pointer<T>(std::move(allocation));
	}

	template <typename A, typename B>
	bool operator==(const Pointer<A>& a, const Pointer<B>& b) noexcept
	{
		return a.get() == b.get();
	}

	template <typename A, typename B>
	bool operator!=(const Pointer<A>& a, const Pointer<B>& b) noexcept
	{
		return !(a == b);
	}
}

#endif
