/// \file
/// \brief %Yttrium allocator for standard containers.

#ifndef _include_yttrium_std_allocator_h_
#define _include_yttrium_std_allocator_h_

#include <yttrium/allocator.h>

namespace Yttrium
{
	template <typename T>
	class StdAllocator
	{
	public:

		using value_type = T;

		StdAllocator() = delete; // For better diagnostics.

		StdAllocator(Allocator& a)
			: _allocator(&a)
		{
		}

		template <typename>
		friend class StdAllocator;

		template <class U>
		StdAllocator(const StdAllocator<U>& a) noexcept
			: _allocator(a._allocator)
		{
		}

		T* allocate(size_t n)
		{
			return _allocator->allocate<T>(n);
		}

		void deallocate(T* p, size_t) noexcept
		{
			_allocator->deallocate(p);
		}

		template <typename T1, typename T2>
		friend bool operator==(const StdAllocator<T1>&, const StdAllocator<T2>&) noexcept;

	private:
		Allocator* _allocator;
	};

	template <typename T1, typename T2>
	bool operator==(const StdAllocator<T1>& lhs, const StdAllocator<T2>& rhs) noexcept
	{
		return lhs._allocator == rhs._allocator;
	}

	template <typename T1, typename T2>
	bool operator!=(const StdAllocator<T1>& lhs, const StdAllocator<T2>& rhs) noexcept
	{
		return !(lhs == rhs);
	}
}

#endif
