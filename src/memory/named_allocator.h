#ifndef _src_memory_named_allocator_list_h_
#define _src_memory_named_allocator_list_h_

#include <yttrium/memory/unique_ptr.h>
#include <yttrium/string.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

namespace Yttrium
{
	class BufferMemory;
	class NamedAllocator;
	struct NamedAllocatorInfo;

	class NamedAllocatorData
	{
	public:
		NamedAllocatorData(std::string&& name)
			: _name(std::move(name))
		{
		}

		StaticString name() const { return StaticString{ _name }; }

		void allocate(size_t size)
		{
			_allocations.fetch_add(1, std::memory_order_relaxed);
			_allocated_blocks.fetch_add(1, std::memory_order_relaxed);
			_allocated_bytes.fetch_add(size, std::memory_order_relaxed);
		}

		void deallocate(size_t size)
		{
			_allocated_blocks.fetch_sub(1, std::memory_order_relaxed);
			_allocated_bytes.fetch_sub(size, std::memory_order_relaxed);
		}

		void reallocate(size_t old_size, size_t new_size)
		{
			_allocated_bytes.fetch_add(new_size - old_size, std::memory_order_relaxed);
			_reallocations.fetch_add(1, std::memory_order_relaxed);
		}

		void store(NamedAllocatorInfo&);

	private:
		std::atomic<size_t> _allocations{0};
		std::atomic<size_t> _allocated_blocks{0};
		std::atomic<size_t> _allocated_bytes{0};
		std::atomic<size_t> _reallocations{0};
		const std::string _name;
	};

	class NamedAllocators
	{
	public:
		NamedAllocators(Allocator&);
		~NamedAllocators();

	private:
		NamedAllocatorData* data(std::string&&);
		void enumerate(std::vector<NamedAllocatorInfo>&) const;

	private:
		Allocator& _allocator;
		std::vector<std::unique_ptr<NamedAllocatorData>> _list;
		mutable std::mutex _mutex;
		friend BufferMemory;
		friend NamedAllocator;
	};

	extern NamedAllocators _named_allocators;
}

#endif
