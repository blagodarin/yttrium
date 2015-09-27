/// \file
/// \brief %Allocator.

#ifndef _include_yttrium_allocator_h_
#define _include_yttrium_allocator_h_

#include <yttrium/global.h>

#include <cstddef>
#include <new>

namespace Yttrium
{
	/// Common allocator interface.
	class Allocator
	{
	public:

		/// Relocatability flag.
		enum Movability
		{
			MayMove,    ///< The memory may be relocated.
			MayNotMove, ///< The memory may not be relocated, and the function may return \c nullptr.
		};

		///
		struct Difference
		{
			///
			enum Direction
			{
				Increment, ///<
				Decrement, ///<
			};

			size_t    allocated; ///<
			size_t    total;     ///<
			Direction direction; ///<

			///
			Difference(size_t allocated = 0, size_t total = 0, Direction direction = Increment)
				: allocated(allocated)
				, total(total)
				, direction(direction)
			{
			}
		};

	public:

		///
		void* allocate(size_t size, size_t alignment = 0, Difference* difference = nullptr)
		{
			return do_allocate(size, alignment, difference);
		}

		///
		void deallocate(void* pointer, Difference* difference = nullptr)
		{
			if (pointer)
				do_deallocate(pointer, difference);
		}

		///
		void* reallocate(void* pointer, size_t size, Movability movability = MayMove, Difference* difference = nullptr)
		{
			return do_reallocate(pointer, size, movability, difference);
		}

		///
		template <typename T>
		Y_PRIVATE T* allocate(size_t count = 1)
		{
			return static_cast<T*>(allocate(sizeof(T) * count));
		}

		///
		template <typename T>
		void delete_(T* pointer)
		{
			if (pointer)
			{
				pointer->~T();
				do_deallocate(pointer, nullptr);
			}
		}

	protected:

		Allocator() = default;
		virtual ~Allocator() = default;

		virtual void* do_allocate(size_t, size_t, Difference*) = 0;
		virtual void do_deallocate(void*, Difference*) = 0;
		virtual void* do_reallocate(void*, size_t, Movability, Difference*) = 0;
	};
}

///
#define Y_DELETE(allocator, pointer) (allocator)->delete_(pointer)

///
#define Y_NEW(allocator, Class) new(static_cast<Yttrium::Allocator*>(allocator)->allocate<Class>()) Class

#endif
