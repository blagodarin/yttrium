#ifndef _src_memory_raw_h_
#define _src_memory_raw_h_

#include <yttrium/memory/allocation.h>

#include <new>
#include <utility>

namespace Yttrium
{
	template <typename T, typename... Args>
	T* make_raw(Allocator& allocator, Args&&... args)
	{
		Allocation<T> allocation(allocator);
		new(allocation.get()) T(std::forward<Args>(args)...);
		return allocation.release();
	}

	template <typename T, typename... Args>
	T* make_raw_sized(Allocator& allocator, size_t size, Args&&... args)
	{
		Allocation<T> allocation(allocator, size);
		new(allocation.get()) T(std::forward<Args>(args)...);
		return allocation.release();
	}

	template <typename T>
	void unmake_raw(Allocator& allocator, T* pointer)
	{
		if (pointer)
		{
			pointer->~T();
			allocator.deallocate(pointer);
		}
	}
}

#endif
