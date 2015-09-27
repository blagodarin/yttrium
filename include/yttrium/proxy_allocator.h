/// \file
/// \brief Proxy allocator.

#ifndef _include_yttrium_proxy_allocator_h_
#define _include_yttrium_proxy_allocator_h_

#include <yttrium/allocator.h>
#include <yttrium/static_string.h>

namespace Yttrium
{
	/// Memory status.
	struct MemoryStatus
	{
		size_t allocated_blocks = 0; ///< Number of memory blocks allocated.
		size_t allocated_bytes = 0;  ///< Allocated memory size.
		size_t total_bytes = 0;      ///< Used memory size.

		size_t allocations = 0;      ///< Lifetime (wrapping) number of allocations.
		size_t reallocations = 0;    ///< Lifetime (wrapping) number of reallocations.
		size_t deallocations = 0;    ///< Lifetime (wrapping) number of deallocations.
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

	private:

		void* do_allocate(size_t, size_t, Difference*) override;
		void do_deallocate(void*, Difference*) override;
		void* do_reallocate(void*, size_t, Movability, Difference*) override;

	private:

		class Private;
		Private* _private;
	};
}

#endif
