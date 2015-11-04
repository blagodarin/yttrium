/// \file
/// \brief %Allocator.

#ifndef _include_yttrium_allocator_h_
#define _include_yttrium_allocator_h_

#include <yttrium/global.h>

#include <cstddef>
#include <new>

namespace Yttrium
{
	/// Memory allocator interface.
	class Allocator
	{
	public:

		///
		void* allocate(size_t size, size_t alignment = 1)
		{
			return do_allocate(size, alignment);
		}

		///
		template <typename T>
		Y_PRIVATE T* allocate(size_t count = 1)
		{
			return static_cast<T*>(allocate(sizeof(T) * count, alignof(T)));
		}

		/// Deallocates an allocated memory block.
		/// \param pointer Pointer to the allocated memory (may be \c nullptr).
		/// \param reallocation \c true if this deallocation terminates memory reallocation.
		void deallocate(void* pointer, bool reallocation = false) noexcept
		{
			if (pointer)
				do_deallocate(pointer, reallocation);
		}

		///
		template <typename T>
		void delete_(T* pointer) noexcept
		{
			if (pointer)
			{
				pointer->~T();
				do_deallocate(pointer, false);
			}
		}

	protected:

		Allocator() = default;
		virtual ~Allocator() = default;

		virtual void* do_allocate(size_t, size_t) = 0;
		virtual void do_deallocate(void*, bool) noexcept = 0;
	};
}

///
#define Y_DELETE(allocator, pointer) (allocator)->delete_(pointer)

///
#define Y_NEW(allocator, Class) new(static_cast<Yttrium::Allocator*>(allocator)->allocate<Class>()) Class

#endif
