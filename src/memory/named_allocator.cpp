#include <yttrium/memory/named_allocator.h>

#include <yttrium/string_format.h>
#include <yttrium/memory/temporary_allocator.h>
#include "heap_allocator.h"
#include "named_allocator.h"

#include <iostream>

namespace Yttrium
{
	void NamedAllocatorData::store(NamedAllocatorInfo& info)
	{
		info.name = _name;
		info.allocations = _allocations.load(std::memory_order_relaxed);
		info.blocks = _allocated_blocks.load(std::memory_order_relaxed);
		info.bytes = _allocated_bytes.load(std::memory_order_relaxed);
		info.reallocations = _reallocations.load(std::memory_order_relaxed);
	}

	NamedAllocators::NamedAllocators(Allocator& allocator)
		: _allocator(allocator)
		, _list(_allocator)
	{
	}

	NamedAllocators::~NamedAllocators()
	{
		bool memory_leaks = false;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			for (const auto& data : _list)
			{
				NamedAllocatorInfo info;
				data->store(info);
				if (!info.blocks)
					continue;
				TemporaryAllocator<128> allocator(NoAllocator);
				String message(&allocator);
				message << "Memory leak detected in \""_s << info.name << "\" ("_s << info.blocks << " blocks"_s;
				if (info.bytes)
					message << ", "_s << info.bytes << " bytes"_s;
				message << ")"_s;
				std::cerr << message << std::endl; // LogManager is already destroyed at this point.
				memory_leaks = true;
			}
		}
		if (memory_leaks)
			std::abort();
	}

	NamedAllocatorData* NamedAllocators::data(String&& name)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		for (const auto& data : _list)
			if (data->name() == name)
				return data.get();
		_list.emplace_back(make_unique<NamedAllocatorData>(_allocator, std::move(name)));
		return _list.back().get();
	}

	void NamedAllocators::enumerate(StdVector<NamedAllocatorInfo>& infos) const
	{
		std::lock_guard<std::mutex> lock(_mutex);
		infos.resize(_list.size());
		for (const auto& data : _list)
			data->store(infos[&data - &_list.front()]);
	}

	class NamedAllocatorPrivate
	{
	public:
		Allocator& _allocator;
		NamedAllocatorData* const _data;

		NamedAllocatorPrivate(Allocator& allocator, NamedAllocatorData* data)
			: _allocator(allocator)
			, _data(data)
		{
		}
	};

	NamedAllocator::NamedAllocator(const StaticString& name)
		: _private(make_unique<NamedAllocatorPrivate>(_heap_allocator, _heap_allocator,
			_named_allocators.data(name.is_empty()
				? String("heap"_s, &_heap_allocator)
				: String("heap."_s.size() + name.size(), &_heap_allocator) << "heap."_s << name)))
	{
	}

	void NamedAllocator::enumerate(StdVector<NamedAllocatorInfo>& infos)
	{
		_named_allocators.enumerate(infos);
	}

	NamedAllocator::~NamedAllocator() = default;

	void* NamedAllocator::do_allocate(size_t size, size_t alignment)
	{
		const auto pointer = _private->_allocator.allocate(size, alignment);
		_private->_data->allocate(0);
		return pointer;
	}

	void NamedAllocator::do_deallocate(void* pointer, bool reallocation) noexcept
	{
		_private->_allocator.deallocate(pointer);
		_private->_data->deallocate(0);
		if (reallocation)
			_private->_data->reallocate(0, 0);
	}
}
