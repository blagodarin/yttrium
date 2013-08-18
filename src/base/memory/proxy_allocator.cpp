#include <yttrium/proxy_allocator.h>

#include <yttrium/memory_manager.h>
#include <yttrium/string.h>

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

	Private(const String &name, Allocator *allocator)
		: _allocator(allocator)
		, _name(name)
	{
	}

public:

	Allocator          *_allocator;
	String              _name;
	AtomicMemoryStatus  _status;

#if Y_IS_DEBUG
	std::map<void *, size_t> _pointers;
#endif
};

ProxyAllocator::ProxyAllocator(const StaticString &name, Allocator *allocator)
	: _private(nullptr)
{
	if (!allocator)
	{
		allocator = MemoryManager::default_allocator();
	}

	ProxyAllocator *proxy_allocator = dynamic_cast<ProxyAllocator *>(allocator);

	const String &final_name = proxy_allocator
		? String(proxy_allocator->name(), allocator).append('.').append(name)
		: String(name, allocator);

	_private = Y_NEW(allocator, Private)(final_name, allocator);
}

ProxyAllocator::~ProxyAllocator()
{
	// We can't use Y_ASSERT here because it uses logging which may already be shut down,
	// so the only remaining way is to write the message to std::cerr.

	const StaticString &name = _private->_name;
	const MemoryStatus &status = _private->_status;

#if Y_IS_DEBUG
	std::cerr
		<< std::left
		<< ":: " << std::setw(16) << name.text()
		<< std::right
		<< std::setw(5) << status.allocations << " a "
		<< std::setw(5) << status.reallocations << " r "
		<< std::setw(5) << status.deallocations << " d "
		<< std::setw(5) << status.allocated_blocks << " l"
		<< std::endl;

	if (status.allocated_blocks)
	{
		int column = 0;
		for (std::map<void *, size_t>::iterator i = _private->_pointers.begin(); i != _private->_pointers.end(); ++i)
		{
			std::cerr << (column ? "  " : "\n\t") << std::setw(10) << i->first << "," << i->second;
			column = (column + 1) % 10;
		}
		std::cerr << std::endl;
	}
#endif

	Y_DELETE(_private->_allocator, _private);
}

Allocator *ProxyAllocator::allocator() const
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

void *ProxyAllocator::allocate(size_t size, size_t align, Difference *difference)
{
	Difference local_difference;

	if (!difference)
	{
		difference = &local_difference;
	}

	void *pointer = _private->_allocator->allocate(size, align, difference);
	_private->_status.allocate(*difference);

#if Y_IS_DEBUG
	_private->_pointers[pointer] = size;
#endif

	return pointer;
}

void ProxyAllocator::deallocate(void *pointer, Difference *difference)
{
	if (Y_UNLIKELY(!pointer))
	{
		return;
	}

#if Y_IS_DEBUG
	if (_private->_pointers.find(pointer) == _private->_pointers.end())
	{
		::abort();
	}
#endif

	Difference local_difference;

	if (!difference)
	{
		difference = &local_difference;
	}

	_private->_allocator->deallocate(pointer, difference);
	_private->_status.deallocate(*difference);

#if Y_IS_DEBUG
	_private->_pointers.erase(pointer);
#endif
}

void *ProxyAllocator::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
{
	Difference local_difference;

	if (!difference)
	{
		difference = &local_difference;
	}

	void *new_pointer = _private->_allocator->reallocate(pointer, size, movability, difference);

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

} // namespace Yttrium
