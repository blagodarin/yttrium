#ifndef _src_memory_sized_allocation_h_
#define _src_memory_sized_allocation_h_

#include <yttrium/memory/allocator.h>

#include <new>
#include <utility>

namespace Yttrium
{
	template <typename T>
	class SizedAllocation
	{
	public:
		SizedAllocation() = delete;
		SizedAllocation(const SizedAllocation&) = delete;
		SizedAllocation(SizedAllocation&&) = delete;
		SizedAllocation& operator=(const SizedAllocation&) = delete;
		SizedAllocation& operator=(SizedAllocation&&) = delete;

		SizedAllocation(Allocator& allocator, size_t size)
			: _allocator(&allocator)
			, _pointer(static_cast<T*>(_allocator->allocate(size)))
		{
		}

		~SizedAllocation()
		{
			_allocator->deallocate(_pointer);
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

	private:
		Allocator* _allocator = nullptr;
		T* _pointer = nullptr;
	};

	template <typename T, typename... Args>
	T* make_sized(Allocator& allocator, size_t size, Args&&... args)
	{
		SizedAllocation<T> allocation(allocator, size);
		new(allocation.get()) T(std::forward<Args>(args)...);
		return allocation.release();
	}
}

#endif
