/// \file
/// \brief

#ifndef __Y_POOL_HPP
#define __Y_POOL_HPP

#include <Yttrium/allocator.hpp>

namespace Yttrium
{

///
/// \note PoolAllocator is not thread-safe.

class PoolAllocator: public Allocator
{
public:

	///

	struct PoolStatus
	{
		size_t allocated_items;  ///<
		size_t allocated_chunks; ///<
	};

public:

	///
	/// \param item_size Item size in bytes.
	/// \param chunk_size Chunk size in items.

	PoolAllocator(size_t item_size, size_t chunk_size, Allocator *allocator = HeapAllocator::instance())
		: _item_size(item_size)
		, _chunk_size(chunk_size)
		, _allocator(allocator)
	{
	}

	///

	virtual ~PoolAllocator() throw();

	///

	size_t chunk_size() const throw()
	{
		return _chunk_size;
	}

	///

	size_t item_size() const throw()
	{
		return _item_size;
	}

	///

	PoolStatus pool_status() const throw()
	{
		return _pool_status;
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

	size_t      _item_size;
	size_t      _chunk_size;
	Allocator  *_allocator;
	Status      _status;
	PoolStatus  _pool_status;
};

///

template <typename T, size_t N = 64>
class Pool
{
public:

	///

	Pool(Allocator *allocator = HeapAllocator::instance())
		: _allocator(sizeof(T), N, allocator)
	{
	}

	///

	PoolAllocator *allocator() throw()
	{
		return &_allocator;
	}

private:

	PoolAllocator _allocator;
};

} // namespace Yttrium

#endif // __Y_POOL_HPP
