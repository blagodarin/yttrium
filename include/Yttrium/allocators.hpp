/// \file
/// \brief Memory allocators.

#ifndef __Y_ALLOCATORS_HPP
#define __Y_ALLOCATORS_HPP

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

		Status()
			: allocated_blocks(0)
			, allocated_bytes(0)
			, total_bytes(0)
			, allocations(0)
			, reallocations(0)
			, deallocations(0)
		{
		}
	};

public:

	///

	virtual void *allocate(size_t size, size_t align = 0) = 0;

	///

	virtual void deallocate(void *pointer) throw() = 0;

	///

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove) = 0;

	///

	virtual Status status() const throw() = 0;

public:

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

class Y_API HeapAllocator
	: public Allocator
{
public:

	static HeapAllocator *instance();
};

/// System allocator.

class Y_API SystemAllocator
	: public Allocator
{
public:

	///

	virtual size_t lower_bound(size_t size) const throw() = 0;

	///

	virtual size_t upper_bound(size_t size) const throw() = 0;

public:

	///

	static SystemAllocator *instance();
};

/// Proxy allocator.

class Y_API ProxyAllocator
	: public Allocator
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

	virtual void *allocate(size_t size, size_t align = 0);

	///

	virtual void deallocate(void *pointer) throw();

	///

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove);

	///

	virtual Status status() const throw();

private:

	Allocator    *_allocator;
	Status       _status;
	StaticString _name;
};

} // namespace Yttrium

#endif // __Y_ALLOCATORS_HPP
