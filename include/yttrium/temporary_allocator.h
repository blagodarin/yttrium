/// \file
/// \brief Temporary memory allocator.

#ifndef _include_yttrium_temporary_allocator_h_
#define _include_yttrium_temporary_allocator_h_

#include <yttrium/allocator.h>

namespace Yttrium
{
	template <typename T>
	T* align_forward(T* pointer, size_t alignment)
	{
		auto offset = reinterpret_cast<uintptr_t>(pointer);
		if (alignment)
		{
			const auto remainder = offset % alignment;
			if (remainder)
				offset += (alignment - remainder);
		}
		return reinterpret_cast<T*>(offset);
	}

	/// Temporary memory allocator.
	template <size_t Size>
	class TemporaryAllocator : public Allocator
	{
	public:

		///
		explicit TemporaryAllocator(Allocator* parent) : _parent(parent) {}

		TemporaryAllocator(const TemporaryAllocator&) = delete;
		TemporaryAllocator& operator=(const TemporaryAllocator&) = delete;

	private:

		Allocator* const _parent;
		size_t _allocated = 0;
		uint8_t _buffer[Size];

		void* do_allocate(size_t size, size_t alignment, Difference* difference) override
		{
			const auto pointer = align_forward(&_buffer[_allocated], alignment);
			if (pointer + size > &_buffer[Size])
				return _parent->allocate(size, alignment, difference);
			_allocated = pointer + size - &_buffer[0];
			return pointer;
		}

		void do_deallocate(void* pointer, Difference* difference) override
		{
			if (pointer < &_buffer[0] || pointer >= &_buffer[Size])
				_parent->deallocate(pointer, difference);
		}

		void* do_reallocate(void* pointer, size_t size, Movability movability, Difference* difference) override
		{
			if (pointer < &_buffer[0] || pointer >= &_buffer[Size])
				return _parent->reallocate(pointer, size, movability, difference);
			if (movability != MayMove)
				return nullptr;
			throw std::bad_alloc(); // TODO: Implement?
		}
	};
}

#endif
