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
		MayNotMove, ///< The memory may not be relocated.
	};

	///

	struct Difference
	{
		size_t allocated;
		size_t total;
		bool   positive;

		Difference &set(size_t allocated, size_t total, bool positive) throw()
		{
			this->allocated = allocated;
			this->total = total;
			this->positive = positive;

			return *this;
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

		Status() throw();
	};

	/// Atomic allocator status.

	class AtomicStatus
	{
	public:

		AtomicStatus() throw();

		void allocate(const Difference &difference) throw();

		void deallocate(const Difference &difference) throw();

		void reallocate(const Difference &difference) throw();

	public:

		operator Status() const throw();

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

	virtual void *allocate(size_t size, size_t align = 0, Difference *difference = nullptr) = 0;

	///

	virtual void deallocate(void *pointer, Difference *difference = nullptr) throw() = 0;

	///

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove, Difference *difference = nullptr) = 0;

	///

	virtual Status status() const throw() = 0;

public:

	///

	template <typename T>
	T *allocate_(size_t count = 1)
	{
		return static_cast<T *>(allocate(sizeof(T) * count));
	}

	///

	template <typename T>
	void delete_(T *pointer) throw()
	{
		if (pointer)
		{
			pointer->T::~T();
			deallocate(pointer);
		}
	}

	///

	template <typename T>
	T *new_()
	{
		T *pointer = static_cast<T *>(allocate(sizeof(T)));
		new(pointer) T();
		return pointer;
	}

	///

	template <typename T, typename U>
	T *new_(const U& source)
	{
		T *p = static_cast<T *>(allocate(sizeof(T)));
		new(p) T(source);
		return p;
	}

protected:

	///

	virtual ~Allocator() throw()
	{
	}
};

/// Heap allocator.

class Y_API HeapAllocator: public Allocator
{
public:

	static HeapAllocator *instance() throw();
};

/// System allocator.

class Y_API SystemAllocator: public Allocator
{
public:

	///

	virtual size_t lower_bound(size_t size) const throw() = 0;

	///

	virtual size_t upper_bound(size_t size) const throw() = 0;

public:

	///

	static SystemAllocator *instance() throw();
};

/// Proxy allocator.

class Y_API ProxyAllocator: public Allocator
{
public:

	///

	ProxyAllocator(Allocator *allocator, const StaticString &name = StaticString());

	///

	StaticString name() const throw()
	{
		return _name;
	}

public: // Allocator

	///

	virtual void *allocate(size_t size, size_t align = 0, Difference *difference = nullptr);

	///

	virtual void deallocate(void *pointer, Difference *difference = nullptr) throw();

	///

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove, Difference *difference = nullptr);

	///

	virtual Status status() const throw();

private:

	Allocator    *_allocator;
	AtomicStatus  _status;
	StaticString  _name;
};

/// An allocator-managed object with overloaded \c new and \c delete operators.

class Y_API Allocatable
{
public:

	virtual ~Allocatable() throw()
	{
	}

	static void *operator new(size_t size, Allocator *allocator = HeapAllocator::instance());

	static void operator delete(void *pointer) throw();

	Allocator *allocator() throw();
};

} // namespace Yttrium

#endif // __Y_ALLOCATORS_HPP
