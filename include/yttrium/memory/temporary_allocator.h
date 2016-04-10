/// \file
/// \brief Temporary memory allocator.

#ifndef _include_yttrium_memory_temporary_allocator_h_
#define _include_yttrium_memory_temporary_allocator_h_

#include <yttrium/memory/allocator.h>

namespace Yttrium
{
	///
	template <typename T>
	T* align_forward(T* pointer, size_t alignment)
	{
		auto offset = reinterpret_cast<uintptr_t>(pointer);
		const auto remainder = offset % alignment;
		if (remainder)
			offset += (alignment - remainder);
		return reinterpret_cast<T*>(offset);
	}

	/// Temporary memory allocator.
	template <size_t Size>
	class TemporaryAllocator : public Allocator
	{
	public:

		///
		explicit TemporaryAllocator(Allocator& parent) : _parent(parent) {}

		TemporaryAllocator(const TemporaryAllocator&) = delete;
		TemporaryAllocator& operator=(const TemporaryAllocator&) = delete;

	private:

		Allocator& _parent;
		size_t _allocated = 0;
		uint8_t _buffer[Size];

		void* do_allocate(size_t size, size_t alignment) override
		{
			const auto pointer = align_forward(&_buffer[_allocated], alignment);
			if (pointer + size > &_buffer[Size])
				return _parent.allocate(size, alignment);
			_allocated = pointer + size - &_buffer[0];
			return pointer;
		}

		void do_deallocate(void* pointer, bool reallocation) noexcept override
		{
			if (pointer < &_buffer[0] || pointer >= &_buffer[Size])
				_parent.deallocate(pointer, reallocation);
		}
	};
}

#endif
