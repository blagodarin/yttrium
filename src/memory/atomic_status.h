#ifndef __MEMORY_ATOMIC_STATUS_H
#define __MEMORY_ATOMIC_STATUS_H

#include <yttrium/proxy_allocator.h>

#include <atomic>

namespace Yttrium
{
	class AtomicMemoryStatus
	{
	public:

		AtomicMemoryStatus();

		void allocate(const Allocator::Difference& difference);
		void deallocate(const Allocator::Difference& difference);
		void reallocate(const Allocator::Difference& difference);

		operator MemoryStatus() const;

	private:

		std::atomic<size_t> _allocated_blocks;
		std::atomic<size_t> _allocated_bytes;
		std::atomic<size_t> _total_bytes;

		std::atomic<size_t> _allocations;
		std::atomic<size_t> _reallocations;
		std::atomic<size_t> _deallocations;
	};
}

#endif // __BASE_MEMORY_ATOMIC_STATUS_H
