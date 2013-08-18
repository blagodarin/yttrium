/// \file
/// \brief Memory allocators.

#ifndef __Y_ALLOCATOR_H
#define __Y_ALLOCATOR_H

#include <yttrium/types.h>

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

		Difference(size_t allocated = 0, size_t total = 0, Direction direction = Increment) noexcept
			: allocated(allocated)
			, total(total)
			, direction(direction)
		{
		}
	};

public:

	///

	virtual void *allocate(size_t size, size_t align = 0, Difference *difference = nullptr) noexcept = 0;

	///

	virtual void deallocate(void *pointer, Difference *difference = nullptr) noexcept = 0;

	///

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove, Difference *difference = nullptr) noexcept = 0;

public:

	///

	template <typename T>
	Y_PRIVATE T *allocate(size_t count = 1) noexcept
	{
		return static_cast<T *>(allocate(sizeof(T) * count));
	}

	///

	template <typename T>
	void delete_(T *pointer) noexcept
	{
		if (pointer)
		{
			pointer->~T();
			deallocate(pointer);
		}
	}

protected:

	///

	virtual ~Allocator()
	{
	}
};

/// System allocator.

class Y_API SystemAllocator
	: public Allocator
{
public:

	///

	virtual size_t lower_bound(size_t size) const noexcept = 0;

	///

	virtual size_t upper_bound(size_t size) const noexcept = 0;

public:

	///

	static SystemAllocator *instance() noexcept;
};

} // namespace Yttrium

///

#define Y_DELETE(allocator, pointer) (allocator)->delete_(pointer)

///

#define Y_NEW(allocator, Class) new((allocator)->allocate<Class>()) Class

#endif // __Y_ALLOCATOR_H
