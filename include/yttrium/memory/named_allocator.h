/// \file
/// \brief Named allocator.

#ifndef _include_yttrium_memory_named_allocator_h_
#define _include_yttrium_memory_named_allocator_h_

#include <yttrium/memory/unique_ptr.h>

namespace Yttrium
{
	class StaticString;

	/// Named allocator.
	class Y_API NamedAllocator : public Allocator
	{
	public:

		///
		NamedAllocator(const StaticString& name, Allocator& = *DefaultAllocator);

		///
		StaticString name() const noexcept;

	private:
		UniquePtr<class NamedAllocatorPrivate> _private;

	public:
		~NamedAllocator() override;

	private:
		void* do_allocate(size_t, size_t) override;
		void do_deallocate(void*, bool) noexcept override;
	};
}

#endif
