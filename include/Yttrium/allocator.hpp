/// \file
/// \brief Memory allocators.

#ifndef __Y_ALLOCATORS_HPP
#define __Y_ALLOCATORS_HPP

#include <atomic> // atomic_*
#include <new>    // new

#include <Yttrium/static_string.hpp>
#include <Yttrium/types.hpp>

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
		///<

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
	T *allocate(size_t count = 1) noexcept
	{
		return static_cast<T *>(allocate(sizeof(T) * count));
	}

	///

	template <typename T>
	void delete_(T *pointer) noexcept
	{
		if (pointer)
		{
			pointer->T::~T();
			deallocate(pointer);
		}
	}

	///

	template <typename T>
	T *new_() noexcept
	{
		T *pointer = static_cast<T *>(allocate(sizeof(T)));
		new(pointer) T();
		return pointer;
	}

	///

	template <typename T, typename U>
	T *new_(const U& source) noexcept
	{
		T *p = static_cast<T *>(allocate(sizeof(T)));
		new(p) T(source);
		return p;
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

/// Heap allocator.

class Y_API HeapAllocator: public Allocator
{
public:

	static HeapAllocator *instance() noexcept;
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

/// Proxy allocator.

class Y_API ProxyAllocator: public Allocator
{
public:

	///

	ProxyAllocator(Allocator *allocator, const StaticString &name = StaticString()) noexcept;

	///

	StaticString name() const noexcept
	{
		return _name;
	}

public: // Allocator

	///

	virtual void *allocate(size_t size, size_t align = 0, Difference *difference = nullptr) noexcept;

	///

	virtual void deallocate(void *pointer, Difference *difference = nullptr) noexcept;

	///

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove, Difference *difference = nullptr) noexcept;

private:

	Allocator    *_allocator;
	StaticString  _name;
};

/// An allocator-managed object with overloaded \c new and \c delete operators.

class Y_API Allocatable
{
public:

	virtual ~Allocatable() noexcept
	{
	}

	static void *operator new(size_t size, Allocator *allocator = HeapAllocator::instance()) noexcept;

	static void operator delete(void *pointer) noexcept;

	Allocator *allocator() noexcept;
};

} // namespace Yttrium

#endif // __Y_ALLOCATORS_HPP
