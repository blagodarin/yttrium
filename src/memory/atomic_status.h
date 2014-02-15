#ifndef __MEMORY_ATOMIC_STATUS_H
#define __MEMORY_ATOMIC_STATUS_H

#include <yttrium/proxy_allocator.h>

#include <atomic>

namespace Yttrium
{

/// Atomic memory status.

class AtomicMemoryStatus
{
public:

	///

	AtomicMemoryStatus() noexcept;

public:

	///

	void allocate(const Allocator::Difference &difference) noexcept;

	///

	void deallocate(const Allocator::Difference &difference) noexcept;

	///

	void reallocate(const Allocator::Difference &difference) noexcept;

public:

	///

	operator MemoryStatus() const noexcept;

private:

	std::atomic<size_t> _allocated_blocks;
	std::atomic<size_t> _allocated_bytes;
	std::atomic<size_t> _total_bytes;

	std::atomic<size_t> _allocations;
	std::atomic<size_t> _reallocations;
	std::atomic<size_t> _deallocations;
};

} // namespace Yttrium

#endif // __BASE_MEMORY_ATOMIC_STATUS_H
