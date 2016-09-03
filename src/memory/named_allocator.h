#ifndef _src_memory_named_allocator_list_h_
#define _src_memory_named_allocator_list_h_

#include <yttrium/std/vector.h>

#include <mutex>

namespace Yttrium
{
	struct NamedAllocatorData;
	struct NamedAllocatorInfo;

	class NamedAllocators
	{
	public:
		NamedAllocators(Allocator&);
		~NamedAllocators();

	private:
		NamedAllocatorData* data(String&&);
		void enumerate(StdVector<NamedAllocatorInfo>&) const;

	private:
		Allocator& _allocator;
		StdVector<UniquePtr<NamedAllocatorData>> _list;
		mutable std::mutex _mutex;
		friend NamedAllocator;
	};

	extern NamedAllocators _named_allocators;
}

#endif
