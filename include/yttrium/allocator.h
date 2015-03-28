/// \file
/// \brief %Allocator.

#ifndef __Y_ALLOCATOR_H
#define __Y_ALLOCATOR_H

#include <yttrium/types.h>

#include <cstddef>
#include <new>

namespace Yttrium
{

/// Common allocator interface.

class Y_API Allocator
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

	virtual void *allocate(size_t size, size_t align = 0, Difference *difference = nullptr) = 0;

	///

	virtual void deallocate(void *pointer, Difference *difference = nullptr) = 0;

	///

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove, Difference *difference = nullptr) = 0;

public:

	///

	template <typename T>
	Y_PRIVATE T *allocate(size_t count = 1)
	{
		return static_cast<T *>(allocate(sizeof(T) * count));
	}

	///

	template <typename T>
	void delete_(T *pointer)
	{
		if (pointer)
		{
			pointer->~T();
			deallocate(pointer);
		}
	}

protected:

	Allocator() {}
	virtual ~Allocator() {}
};

} // namespace Yttrium

///

#define Y_DELETE(allocator, pointer) (allocator)->delete_(pointer)

///

#define Y_NEW(allocator, Class) new(static_cast<Yttrium::Allocator *>(allocator)->allocate<Class>()) Class

#endif // __Y_ALLOCATOR_H
