/// \file
/// \brief Proxy allocator.

#ifndef _include_yttrium_proxy_allocator_h_
#define _include_yttrium_proxy_allocator_h_

#include <yttrium/allocator.h>
#include <yttrium/static_string.h>

namespace Yttrium
{
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

	private:

		void* do_allocate(size_t, size_t) override;
		void do_deallocate(void*, bool) override;

	private:

		class Private;
		Private* _private;
	};
}

#endif
