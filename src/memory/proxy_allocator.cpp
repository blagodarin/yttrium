#include <yttrium/memory/proxy_allocator.h>

#include <yttrium/string.h>
#include <yttrium/string_format.h>

#include <atomic>

#if Y_IS_DEBUG
	#include <yttrium/std/unordered_map.h>
	#include <iomanip>
	#include <iostream>
#endif

namespace Yttrium
{
	class ProxyAllocatorPrivate
	{
	public:
		ProxyAllocatorPrivate(Allocator& allocator, String&& name)
			: _allocator(allocator)
			, _name(std::move(name))
		#if Y_IS_DEBUG
			, _pointers(_allocator)
		#endif
		{
		}

	public:
		Allocator& _allocator;
		const String _name;
		std::atomic<size_t> _allocations{0};
		std::atomic<size_t> _reallocations{0};
		std::atomic<size_t> _allocated_blocks{0};
	#if Y_IS_DEBUG
		StdUnorderedMap<void*, size_t> _pointers;
	#endif
	};

	ProxyAllocator::ProxyAllocator(const StaticString& name, Allocator& allocator)
	{
		const auto proxy_allocator = dynamic_cast<ProxyAllocator*>(&allocator);
		if (proxy_allocator)
		{
			const auto& proxy_name = proxy_allocator->name();
			_private = make_unique<ProxyAllocatorPrivate>(allocator, proxy_allocator->_private->_allocator,
				String(proxy_name.size() + 1 + name.size(), &allocator) << proxy_name << '.' << name);
		}
		else
			_private = make_unique<ProxyAllocatorPrivate>(allocator, allocator, String(name, &allocator));
	}

	StaticString ProxyAllocator::name() const noexcept
	{
		return _private->_name;
	}

	ProxyAllocator::~ProxyAllocator()
	{
	#if Y_IS_DEBUG
		const auto allocations = _private->_allocations.load();
		const auto reallocations = _private->_reallocations.load();
		const auto allocated_blocks = _private->_allocated_blocks.load();

		std::cerr
			<< std::left
			<< ":: "
			<< std::right
			<< std::setw(6) << allocations << " a "
			<< std::setw(6) << reallocations << " r "
			<< "::  "
			<< _private->_name
			<< std::endl;

		if (allocated_blocks)
		{
			int column = 0;
			for (const auto& pointer : _private->_pointers)
			{
				std::cerr << (column ? "  " : "\n\t") << std::setw(10) << pointer.first << "," << pointer.second;
				column = (column + 1) % 10;
			}
			std::cerr << std::endl;
		}
	#endif
	}

	void* ProxyAllocator::do_allocate(size_t size, size_t alignment)
	{
		const auto pointer = _private->_allocator.allocate(size, alignment);
		++_private->_allocated_blocks;
		++_private->_allocations;
	#if Y_IS_DEBUG
		_private->_pointers[pointer] = size;
	#endif
		return pointer;
	}

	void ProxyAllocator::do_deallocate(void* pointer, bool reallocation) noexcept
	{
	#if Y_IS_DEBUG
		if (_private->_pointers.find(pointer) == _private->_pointers.end())
			::abort();
	#endif
		_private->_allocator.deallocate(pointer);
		--_private->_allocated_blocks;
		if (reallocation)
			++_private->_reallocations;
	#if Y_IS_DEBUG
		_private->_pointers.erase(pointer);
	#endif
	}
}
