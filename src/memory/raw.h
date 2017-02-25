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
		RawAllocation(size_t size)
			: _pointer{ std::malloc(size) }
		{
			if (!_pointer)
				throw std::bad_alloc();
		}

		~RawAllocation()
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
	T* make_raw_sized(size_t size, Args&&... args)
	{
		assert(size >= sizeof(T));
		RawAllocation allocation(size);
		new(allocation.get()) T(std::forward<Args>(args)...);
		return static_cast<T*>(allocation.release());
	}
}

#endif
