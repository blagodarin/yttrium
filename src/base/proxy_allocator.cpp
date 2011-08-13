#include <Yttrium/allocators.hpp>

// NOTE: We should be GCC-compatible here.

namespace Yttrium
{

ProxyAllocator::ProxyAllocator(Allocator *allocator, const StaticString &name)
	: _allocator(allocator)
	, _name(name)
{
}

void *ProxyAllocator::allocate(size_t size, size_t align)
{
	Status before = _allocator->status();
	void *pointer = _allocator->allocate(size, align);
	Status after = _allocator->status();

	__sync_add_and_fetch(&_status.total_bytes, after.total_bytes - before.total_bytes);
	__sync_add_and_fetch(&_status.allocated_bytes, after.allocated_bytes - before.allocated_bytes);
	__sync_add_and_fetch(&_status.allocated_blocks, 1);

	__sync_add_and_fetch(&_status.allocations, 1);

	return pointer;
}

void ProxyAllocator::deallocate(void *pointer) throw()
{
	if (pointer)
	{
		Status before = _allocator->status();
		_allocator->deallocate(pointer);
		Status after = _allocator->status();

		__sync_sub_and_fetch(&_status.allocated_blocks, 1);
		__sync_sub_and_fetch(&_status.allocated_bytes, before.allocated_bytes - after.allocated_bytes);
		__sync_sub_and_fetch(&_status.total_bytes, before.total_bytes - after.total_bytes);

		__sync_add_and_fetch(&_status.deallocations, 1);
	}
}

void *ProxyAllocator::reallocate(void *pointer, size_t size, Movability movability)
{
	Status before = _allocator->status();
	void *new_pointer = _allocator->reallocate(pointer, size, movability);
	Status after = _allocator->status();

	if (new_pointer && before.reallocations != after.reallocations)
	{
		if (after.allocated_bytes > before.allocated_bytes)
		{
			__sync_add_and_fetch(&_status.total_bytes, after.total_bytes - before.total_bytes);
			__sync_add_and_fetch(&_status.allocated_bytes, after.allocated_bytes - before.allocated_bytes);
		}
		else
		{
			__sync_sub_and_fetch(&_status.allocated_bytes, before.allocated_bytes - after.allocated_bytes);
			__sync_sub_and_fetch(&_status.total_bytes, before.total_bytes - after.total_bytes);
		}
		__sync_add_and_fetch(&_status.reallocations, 1);
	}

	return new_pointer;
}

Allocator::Status ProxyAllocator::status() const throw()
{
	return _status;
}

} // namespace Yttrium
