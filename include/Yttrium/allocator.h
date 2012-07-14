/// \file
/// \brief Memory allocators.

#ifndef __Y_ALLOCATOR_H
#define __Y_ALLOCATOR_H

#include <Yttrium/types.h>

#include <atomic>
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

	/// %Allocator status.

	struct Status
	{
		size_t allocated_blocks; ///< Number of memory blocks allocated.
		size_t allocated_bytes;  ///< Allocated memory size.
		size_t total_bytes;      ///< Used memory size.

		size_t allocations;      ///< Lifetime (wrapping) number of allocations.
		size_t reallocations;    ///< Lifetime (wrapping) number of reallocations.
		size_t deallocations;    ///< Lifetime (wrapping) number of deallocations.

		///

		Status() noexcept
			: allocated_blocks(0)
			, allocated_bytes(0)
			, total_bytes(0)
			, allocations(0)
			, reallocations(0)
			, deallocations(0)
		{
		}
	};

	/// Atomic allocator status.

	class AtomicStatus
	{
	public:

		///

		AtomicStatus() noexcept;

		///

		void allocate(const Difference &difference) noexcept;

		///

		void deallocate(const Difference &difference) noexcept;

		///

		void reallocate(const Difference &difference) noexcept;

	public:

		///

		operator Status() const noexcept;

	private:

		std::atomic<size_t> _allocated_blocks;
		std::atomic<size_t> _allocated_bytes;
		std::atomic<size_t> _total_bytes;

		std::atomic<size_t> _allocations;
		std::atomic<size_t> _reallocations;
		std::atomic<size_t> _deallocations;
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

	///

	Status status() const noexcept
	{
		return _status;
	}

protected:

	///

	virtual ~Allocator() noexcept
	{
	}

protected:

	AtomicStatus _status;
};

/// System allocator.

class Y_API SystemAllocator: public Allocator
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

/// Root allocator.

extern Y_API Allocator *const DefaultAllocator;

} // namespace Yttrium

#endif // __Y_ALLOCATOR_H
