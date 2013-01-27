#include <Yttrium/proxy_allocator.h>

#include <Yttrium/memory_manager.h>
#include <Yttrium/string.h>

#include <iostream>

#if Y_IS_DEBUG
	#include <iomanip>
	#include <map>
#endif

namespace Yttrium
{

class ProxyAllocator::Private
{
public:

	Private(const String &name, Allocator *allocator)
		: _allocator(allocator)
		, _name(name)
	{
	}

public:

	Allocator *_allocator;
	String     _name;

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
	// NOTE: We can't use Y_ASSERT here because it uses logging which may already be shut down,
	// so the only remaining way is to write the message to std::cerr.

	const StaticString &name = _private->_name;
	const Status &status = _status;

#if Y_IS_DEBUG
	std::cerr << "Memory usage statistics for \"" << name.text() << "\":\n"
		"\tAllocations:   " << status.allocations << "\n"
		"\tReallocations: " << status.reallocations << "\n"
		"\tDeallocations: " << status.deallocations << std::endl;
#endif

	if (status.allocated_blocks)
	{
		std::cerr << "Memory leak detected in \"" << name.text() << "\": "
			<< status.allocated_blocks << " blocks are still allocated!" << std::endl;

	#if Y_IS_DEBUG
		int column = 0;
		for (std::map<void *, size_t>::iterator i = _private->_pointers.begin(); i != _private->_pointers.end(); ++i)
		{
			std::cerr << (column ? "  " : "\n\t") << std::setw(10) << i->first << "," << i->second;
			column = (column + 1) % 10;
		}
		std::cerr << std::endl;
//		::abort();
	#endif
	}

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

void *ProxyAllocator::allocate(size_t size, size_t align, Difference *difference)
{
	Difference local_difference;

	if (!difference)
	{
		difference = &local_difference;
	}

	void *pointer = _private->_allocator->allocate(size, align, difference);
	_status.allocate(*difference);

#if Y_IS_DEBUG
	_private->_pointers[pointer] = size;
#endif

	return pointer;
}

void ProxyAllocator::deallocate(void *pointer, Difference *difference)
{
	if (pointer)
	{
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
		_status.deallocate(*difference);

	#if Y_IS_DEBUG
		_private->_pointers.erase(pointer);
	#endif
	}
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
		_status.reallocate(*difference);

	#if Y_IS_DEBUG
		_private->_pointers.erase(pointer);
		_private->_pointers[new_pointer] = size;
	#endif
	}

	return new_pointer;
}

} // namespace Yttrium
