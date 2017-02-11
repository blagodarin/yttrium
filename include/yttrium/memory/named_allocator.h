/// \file
/// \brief Named allocator.

#ifndef _include_yttrium_memory_named_allocator_h_
#define _include_yttrium_memory_named_allocator_h_

#include <yttrium/memory/allocator.h>
#include <yttrium/static_string.h>

#include <memory>
#include <vector>

namespace Yttrium
{
	///
	struct NamedAllocatorInfo
	{
		StaticString name;
		size_t allocations = 0;
		size_t blocks = 0;
		size_t bytes = 0;
		size_t reallocations = 0;
	};

	/// Named allocator.
	class Y_API NamedAllocator : public Allocator
	{
	public:
		~NamedAllocator() override;

		/// Creates a named allocator for heap memory allocations.
		NamedAllocator(const std::string& name);

		///
		static void enumerate(std::vector<NamedAllocatorInfo>&);

	private:
		void* do_allocate(size_t, size_t) override;
		void do_deallocate(void*, bool) noexcept override;

	private:
		const std::unique_ptr<class NamedAllocatorPrivate> _private;
	};
}

#endif
