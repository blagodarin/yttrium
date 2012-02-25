#include <Yttrium/pool.hpp>

#include <Yttrium/assert.hpp>

namespace Yttrium
{

PoolAllocator::~PoolAllocator()
{
	Y_ASSERT(!_pool_status.allocated_items);
}

void *PoolAllocator::allocate(size_t size, size_t, Difference *difference)
{
	Y_ASSERT(size <= _item_size);

	Y_ABORT("Not implemented");

	return nullptr;
}

void PoolAllocator::deallocate(void *pointer, Difference *difference)
{
}

void *PoolAllocator::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
{
	Y_ASSERT(size <= _item_size);

	Y_ABORT("Not implemented");

	return nullptr;
}

} // namespace Yttrium
