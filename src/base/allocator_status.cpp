#include <Yttrium/allocator.hpp>

namespace Yttrium
{

Allocator::Status::Status() throw()
	: allocated_blocks(0)
	, allocated_bytes(0)
	, total_bytes(0)
	, allocations(0)
	, reallocations(0)
	, deallocations(0)
{
}

Allocator::AtomicStatus::AtomicStatus() throw()
	: _allocated_blocks(0)
	, _allocated_bytes(0)
	, _total_bytes(0)
	, _allocations(0)
	, _reallocations(0)
	, _deallocations(0)
{
}

void Allocator::AtomicStatus::allocate(const Difference &difference) throw()
{
	std::atomic_fetch_add(&_total_bytes, difference.total);
	std::atomic_fetch_add(&_allocated_bytes, difference.allocated);
	std::atomic_fetch_add(&_allocated_blocks, 1u);

	std::atomic_fetch_add(&_allocations, 1u);
}

void Allocator::AtomicStatus::deallocate(const Difference &difference) throw()
{
	std::atomic_fetch_sub(&_allocated_blocks, 1u);
	std::atomic_fetch_sub(&_allocated_bytes, difference.allocated);
	std::atomic_fetch_sub(&_total_bytes, difference.total);

	std::atomic_fetch_add(&_deallocations, 1u);
}

void Allocator::AtomicStatus::reallocate(const Difference &difference) throw()
{
	if (difference.positive)
	{
		std::atomic_fetch_add(&_total_bytes, difference.total);
		std::atomic_fetch_add(&_allocated_bytes, difference.allocated);
	}
	else
	{
		std::atomic_fetch_sub(&_allocated_bytes, difference.allocated);
		std::atomic_fetch_sub(&_total_bytes, difference.total);
	}

	std::atomic_fetch_add(&_reallocations, 1u);
}

Allocator::AtomicStatus::operator Status() const throw()
{
	Status result;

	result.allocated_blocks = _allocated_blocks;
	result.allocated_bytes = _allocated_bytes;
	result.total_bytes = _total_bytes;

	result.allocations = _allocations;
	result.reallocations = _reallocations;
	result.deallocations = _deallocations;

	return result;
}

} // namespace Yttrium
