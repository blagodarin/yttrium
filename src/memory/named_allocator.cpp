#include <yttrium/memory/named_allocator.h>

#include <yttrium/string.h>
#include <yttrium/string_format.h>
#include "heap_allocator.h"
#include "named_allocator.h"

#include <atomic>

#define Y_ENABLE_NAMED_MEMORY_DEBUGGING 0

#if Y_IS_DEBUG
	#include <yttrium/std/unordered_map.h>
	#include <iomanip>
	#include <iostream>
#endif

namespace Yttrium
{
	struct NamedAllocatorData
	{
		const String _name;
		std::atomic<size_t> _allocated_blocks{0};
		std::atomic<size_t> _allocations{0};
		std::atomic<size_t> _reallocations{0};
	#if Y_ENABLE_NAMED_MEMORY_DEBUGGING
		StdUnorderedMap<void*, size_t> _pointers;
	#endif

		NamedAllocatorData(String&& name)
			: _name(std::move(name))
	#if Y_ENABLE_NAMED_MEMORY_DEBUGGING
			, _pointers(*_name.allocator())
	#endif
		{
		}
	};

	NamedAllocators::NamedAllocators(Allocator& allocator)
		: _allocator(allocator)
		, _list(_allocator)
	{
	}

	NamedAllocators::~NamedAllocators()
	{
		bool memory_leak_detected = false;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			for (const auto& data : _list)
			{
				if (data->_allocated_blocks.load())
				{
					memory_leak_detected = true;
				#if Y_ENABLE_NAMED_MEMORY_DEBUGGING
					int column = 0;
					std::cerr << data->_name << ":\n";
					for (const auto& pointer : data->_pointers)
					{
						std::cerr << (column ? "  " : "\n\t") << std::setw(10) << pointer.first << "," << pointer.second;
						column = (column + 1) % 10;
					}
					std::cerr << std::endl;
				#else
					break;
				#endif
				}
			}
		}
		if (memory_leak_detected)
			std::abort();
	}

	NamedAllocatorData* NamedAllocators::data(String&& name)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		for (const auto& data : _list)
			if (data->_name == name)
				return data.get();
		_list.emplace_back(make_unique<NamedAllocatorData>(_allocator, std::move(name)));
		return _list.back().get();
	}

	void NamedAllocators::enumerate(StdVector<NamedAllocatorInfo>& infos) const
	{
		std::lock_guard<std::mutex> lock(_mutex);
		infos.resize(_list.size());
		for (const auto& data : _list)
		{
			auto& info = infos[&data - &_list.front()];
			info.name = data->_name;
			info.blocks = data->_allocated_blocks.load();
			info.allocations = data->_allocations.load();
			info.reallocations = data->_reallocations.load();
		}
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
		++_private->_data->_allocated_blocks;
		++_private->_data->_allocations;
	#if Y_ENABLE_NAMED_MEMORY_DEBUGGING
		_private->_data->_pointers.emplace(pointer, size);
	#endif
		return pointer;
	}

	void NamedAllocator::do_deallocate(void* pointer, bool reallocation) noexcept
	{
	#if Y_ENABLE_NAMED_MEMORY_DEBUGGING
		if (_private->_data->_pointers.find(pointer) == _private->_data->_pointers.end())
			::abort();
	#endif
		_private->_allocator.deallocate(pointer);
		--_private->_data->_allocated_blocks;
		if (reallocation)
			++_private->_data->_reallocations;
	#if Y_ENABLE_NAMED_MEMORY_DEBUGGING
		_private->_data->_pointers.erase(pointer);
	#endif
	}
}
