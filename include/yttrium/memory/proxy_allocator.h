/// \file
/// \brief Proxy allocator.

#ifndef _include_yttrium_memory_proxy_allocator_h_
#define _include_yttrium_memory_proxy_allocator_h_

#include <yttrium/memory/allocator.h>
#include <yttrium/memory/global.h>

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
		~ProxyAllocator() override;

		///
		StaticString name() const;

	private:

		void* do_allocate(size_t, size_t) override;
		void do_deallocate(void*, bool) noexcept override;

	private:

		class Private;
		Private* _private;
	};
}

#endif
