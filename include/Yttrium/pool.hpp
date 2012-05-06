/// \file
/// \brief

#ifndef __Y_POOL_HPP
#define __Y_POOL_HPP

#include <Yttrium/allocator.hpp>
#include <Yttrium/noncopyable.hpp>

namespace Yttrium
{

/// %Pool status.

struct PoolStatus
{
	size_t allocated_items;     ///< Number of allocated items.
	size_t allocated_chunks;    ///< Number of allocated chunks.

	size_t item_allocations;    ///< Lifetime (wrapping) number of item allocations.
	size_t chunk_allocations;   ///< Lifetime (wrapping) number of chunk allocations.

	size_t item_deallocations;  ///< Lifetime (wrapping) number of item deallocations.
	size_t chunk_deallocations; ///< Lifetime (wrapping) number of chunk deallocations.

	///

	PoolStatus() noexcept
		: allocated_items(0)
		, allocated_chunks(0)
		, item_allocations(0)
		, chunk_allocations(0)
		, item_deallocations(0)
		, chunk_deallocations(0)
	{
	}
};

///

class Y_API PoolBase: public Noncopyable
{
public:

	///

	PoolBase(size_t chunk_items, size_t item_size, Allocator *allocator = DefaultAllocator) noexcept;

	///

	~PoolBase() noexcept;

	///

	PoolStatus status() const noexcept;

protected:

	///

	void *allocate() noexcept;

	///

	void deallocate(void *pointer) noexcept;

	///

	void *take() noexcept;

private:

	class Chunk;

private:

	Allocator  *_allocator;
	size_t      _chunk_items;
	size_t      _item_size;
	size_t      _chunk_size;
	Chunk      *_last_chunk;
	PoolStatus  _status;
};

///

template <typename T>
class Pool: public PoolBase
{
public:

	///

	Pool(size_t chunk_items = 32, Allocator *allocator = DefaultAllocator) noexcept
		: PoolBase(chunk_items, sizeof(T), allocator)
	{
	}

	///

	~Pool()
	{
		for (void *pointer = take(); pointer; pointer = take())
		{
			static_cast<T *>(pointer)->T::~T();
			PoolBase::deallocate(pointer);
		}
	}

public:

	///

	T *allocate() noexcept
	{
		return static_cast<T *>(PoolBase::allocate());
	}

	///

	void deallocate(T *pointer) noexcept
	{
		if (pointer)
		{
			pointer->T::~T();
			PoolBase::deallocate(pointer);
		}
	}
};

} // namespace Yttrium

#endif // __Y_POOL_HPP
