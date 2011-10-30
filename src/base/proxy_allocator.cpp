#include <Yttrium/allocator.hpp>

namespace Yttrium
{

ProxyAllocator::ProxyAllocator(Allocator *allocator, const StaticString &name)
	: _allocator(allocator)
	, _name(name)
{
}

void *ProxyAllocator::allocate(size_t size, size_t align, Difference *difference)
{
	Difference local_difference;

	if (!difference)
		difference = &local_difference;
	void *pointer = _allocator->allocate(size, align, difference);
	_status.allocate(*difference);

	return pointer;
}

void ProxyAllocator::deallocate(void *pointer, Difference *difference) throw()
{
	if (pointer)
	{
		Difference local_difference;

		if (!difference)
			difference = &local_difference;
		_allocator->deallocate(pointer, difference);
		_status.deallocate(*difference);
	}
}

void *ProxyAllocator::reallocate(void *pointer, size_t size, Movability movability, Difference *difference)
{
	Difference local_difference;

	if (!difference)
		difference = &local_difference;
	void *new_pointer = _allocator->reallocate(pointer, size, movability, difference);
	if (new_pointer)
		_status.reallocate(*difference);

	return new_pointer;
}

Allocator::Status ProxyAllocator::status() const throw()
{
	return _status;
}

} // namespace Yttrium
