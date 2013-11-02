#include <yttrium/memory_manager.h>

#include <yttrium/proxy_allocator.h>
#include <yttrium/system_allocator.h>

#include "../instance_guard.h"
#include "heap_allocator.h"

namespace Yttrium
{

Allocator *const DefaultAllocator = SystemAllocator::instance();

typedef InstanceGuard<MemoryManager> MemoryManagerGuard;

MemoryManager::MemoryManager()
	: _previous_default_allocator(DefaultAllocator)
	, _default_allocator(Y_NEW(_previous_default_allocator, HeapAllocator)(_previous_default_allocator))
{
	MemoryManagerGuard::enter(this, "Duplicate MemoryManager construction");

	const_cast<Allocator *&>(DefaultAllocator) =
		Y_NEW(_default_allocator, ProxyAllocator)("default", _default_allocator);
}

MemoryManager::~MemoryManager()
{
	Y_DELETE(_default_allocator, DefaultAllocator);
	Y_DELETE(_previous_default_allocator, _default_allocator);
	const_cast<Allocator *&>(DefaultAllocator) = _previous_default_allocator;

	MemoryManagerGuard::leave(this);
}

Allocator *MemoryManager::default_allocator()
{
	return MemoryManagerGuard::instance->_default_allocator;
}

} // namespace Yttrium
