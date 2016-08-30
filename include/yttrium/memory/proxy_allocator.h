/// \file
/// \brief Proxy allocator.

#ifndef _include_yttrium_memory_proxy_allocator_h_
#define _include_yttrium_memory_proxy_allocator_h_

#include <yttrium/memory/unique_ptr.h>

namespace Yttrium
{
	class StaticString;

	/// Proxy allocator.
	class Y_API ProxyAllocator : public Allocator
	{
	public:

		///
		ProxyAllocator(const StaticString& name, Allocator& allocator = *DefaultAllocator);

		///
		StaticString name() const noexcept;

	private:
		UniquePtr<class ProxyAllocatorPrivate> _private;

	public:
		~ProxyAllocator() override;

	private:
		void* do_allocate(size_t, size_t) override;
		void do_deallocate(void*, bool) noexcept override;
	};
}

#endif
