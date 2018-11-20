#ifndef _src_memory_raw_h_
#define _src_memory_raw_h_

#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>

namespace Yttrium
{
	class RawAllocation
	{
	public:
		explicit RawAllocation(size_t size)
			: _pointer{ std::malloc(size) }
		{
			if (!_pointer)
				throw std::bad_alloc{};
		}

		~RawAllocation() noexcept
		{
			std::free(_pointer);
		}

		void* get() const noexcept
		{
			return _pointer;
		}

		void* release() noexcept
		{
			return std::exchange(_pointer, nullptr);
		}

		RawAllocation(const RawAllocation&) = delete;
		RawAllocation& operator=(const RawAllocation&) = delete;

	private:
		void* _pointer = nullptr;
	};

	template <typename T, typename... Args>
	T* new_sized(size_t size, Args&&... args)
	{
		assert(size >= sizeof(T));
		RawAllocation allocation(size);
		new (allocation.get()) T(std::forward<Args>(args)...);
		return static_cast<T*>(allocation.release());
	}

	template <typename T>
	void delete_sized(T* pointer) noexcept
	{
		if (pointer)
		{
			pointer->~T();
			std::free(pointer);
		}
	}
}

#endif
