/// \file
/// \brief Proxy allocator.

#ifndef __Y_PROXY_ALLOCATOR_H
#define __Y_PROXY_ALLOCATOR_H

#include <yttrium/allocator.h>
#include <yttrium/static_string.h>

namespace Yttrium
{
	/// Memory status.
	struct MemoryStatus
	{
		size_t allocated_blocks; ///< Number of memory blocks allocated.
		size_t allocated_bytes;  ///< Allocated memory size.
		size_t total_bytes;      ///< Used memory size.

		size_t allocations;      ///< Lifetime (wrapping) number of allocations.
		size_t reallocations;    ///< Lifetime (wrapping) number of reallocations.
		size_t deallocations;    ///< Lifetime (wrapping) number of deallocations.

		///

		MemoryStatus()
			: allocated_blocks(0)
			, allocated_bytes(0)
			, total_bytes(0)
			, allocations(0)
			, reallocations(0)
			, deallocations(0)
		{
		}
	};

	/// Proxy allocator.
	class Y_API ProxyAllocator : public Allocator
	{
	public:

		///
		ProxyAllocator(const StaticString& name, Allocator* allocator = nullptr);

		///
		~ProxyAllocator() override;

	public:

		///
		Allocator* allocator() const;

		///
		StaticString name() const;

		///
		MemoryStatus status() const;

	public: // Allocator.

		using Allocator::allocate;

		void* allocate(size_t size, size_t align = 0, Difference* difference = nullptr) override;

		void deallocate(void* pointer, Difference* difference = nullptr) override;

		void* reallocate(void* pointer, size_t size, Movability movability = MayMove, Difference* difference = nullptr) override;

	private:

		class Private;

		Private* _private;
	};
}

#endif
