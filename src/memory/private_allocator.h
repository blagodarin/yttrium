#ifndef __MEMORY_PRIVATE_ALLOCATOR_H
#define __MEMORY_PRIVATE_ALLOCATOR_H

#include <yttrium/proxy_allocator.h>

namespace Yttrium
{

class PrivateAllocator
{
public:

	PrivateAllocator(Allocator* allocator, const StaticString& proxy_name = StaticString())
		: _parent_allocator(allocator)
		, _allocator(proxy_name.is_empty()
			? _parent_allocator
			: Y_NEW(_parent_allocator, ProxyAllocator)(proxy_name, _parent_allocator))
	{
	}

	~PrivateAllocator()
	{
		if (_allocator != _parent_allocator)
			Y_DELETE(_parent_allocator, _allocator);
	}

	template <typename T>
	void delete_private(T* private_)
	{
		Y_DELETE(_parent_allocator, private_);
	}

	operator Allocator*() const
	{
		return _allocator;
	}

private:

	Allocator* _parent_allocator;
	Allocator* _allocator;
};

} // namespace Yttrium

#endif // __MEMORY_PRIVATE_ALLOCATOR_H
