#include <Yttrium/pool.hpp>

#include <Yttrium/assert.hpp>

namespace Yttrium
{

PoolAllocator::~PoolAllocator() throw()
{
	Y_ASSERT(!_pool_status.allocated_items);
}

void *PoolAllocator::allocate(size_t size, size_t, Difference *difference)
{
	Y_ASSERT(size <= _item_size);

	throw std::bad_alloc();
}

void PoolAllocator::deallocate(void *pointer, Difference *difference) throw()
{
}

void *PoolAllocator::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
{
	Y_ASSERT(size <= _item_size);

	throw std::bad_alloc();
}

Allocator::Status PoolAllocator::status() const throw()
{
	return _status;
}

} // namespace Yttrium
