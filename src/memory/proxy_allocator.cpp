#include <yttrium/proxy_allocator.h>

#include <yttrium/memory_manager.h>
#include <yttrium/string.h>
#include <yttrium/string_format.h>
#include "atomic_status.h"

#if Y_IS_DEBUG
	#include <iomanip>
	#include <iostream>
	#include <map>
#endif

namespace Yttrium
{
	class Y_PRIVATE ProxyAllocator::Private
	{
	public:

		Private(String&& name, Allocator* allocator)
			: _allocator(allocator)
			, _name(std::move(name))
		{
		}

	public:

		Allocator* const   _allocator;
		const String       _name;
		AtomicMemoryStatus _status;

	#if Y_IS_DEBUG
		std::map<void*, size_t> _pointers;
	#endif
	};

	ProxyAllocator::ProxyAllocator(const StaticString& name, Allocator* allocator)
		: _private(nullptr)
	{
		if (!allocator)
			allocator = MemoryManager::default_allocator();

		const auto proxy_allocator = dynamic_cast<ProxyAllocator*>(allocator);
		if (!proxy_allocator)
		{
			_private = Y_NEW(allocator, Private)(String(name, allocator), allocator);
		}
		else
		{
			const auto& proxy_name = proxy_allocator->name();
			_private = Y_NEW(allocator, Private)(
				String(proxy_name.size() + 1 + name.size(), allocator) << proxy_name << '.' << name,
				allocator);
		}
	}

	ProxyAllocator::~ProxyAllocator()
	{
	#if Y_IS_DEBUG
		const MemoryStatus status = _private->_status;

		std::cerr
			<< std::left
			<< ":: "
			<< std::right
			<< std::setw(5) << status.allocations << " a "
			<< std::setw(5) << status.reallocations << " r "
			<< std::setw(5) << status.deallocations << " d "
			<< std::setw(5) << status.allocated_blocks << " l ::  "
			<< _private->_name
			<< std::endl;

		if (status.allocated_blocks)
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

		Y_DELETE(_private->_allocator, _private);
	}

	Allocator* ProxyAllocator::allocator() const
	{
		return _private->_allocator;
	}

	StaticString ProxyAllocator::name() const
	{
		return _private->_name;
	}

	MemoryStatus ProxyAllocator::status() const
	{
		return _private->_status;
	}

	void* ProxyAllocator::do_allocate(size_t size, size_t alignment, Difference* difference)
	{
		Difference dummy_difference;
		if (!difference)
			difference = &dummy_difference;

		const auto pointer = _private->_allocator->allocate(size, alignment, difference);
		_private->_status.allocate(*difference);
	#if Y_IS_DEBUG
		_private->_pointers[pointer] = size;
	#endif
		return pointer;
	}

	void ProxyAllocator::do_deallocate(void* pointer, Difference* difference)
	{
	#if Y_IS_DEBUG
		if (_private->_pointers.find(pointer) == _private->_pointers.end())
			::abort();
	#endif

		Difference dummy_difference;
		if (!difference)
			difference = &dummy_difference;

		_private->_allocator->deallocate(pointer, difference);
		_private->_status.deallocate(*difference);
	#if Y_IS_DEBUG
		_private->_pointers.erase(pointer);
	#endif
	}

	void* ProxyAllocator::do_reallocate(void* pointer, size_t size, Movability movability, Difference* difference)
	{
		Difference dummy_difference;
		if (!difference)
			difference = &dummy_difference;

		const auto new_pointer = _private->_allocator->reallocate(pointer, size, movability, difference);
		if (new_pointer)
		{
			_private->_status.reallocate(*difference);
		#if Y_IS_DEBUG
			_private->_pointers.erase(pointer);
			_private->_pointers[new_pointer] = size;
		#endif
		}
		return new_pointer;
	}
}
