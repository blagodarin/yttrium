#ifndef _src_memory_atomic_status_h_
#define _src_memory_atomic_status_h_

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

#endif
