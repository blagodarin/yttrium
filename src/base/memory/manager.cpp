#include <Yttrium/memory_manager.h>

#include "../instance_guard.h"

#include "heap_allocator.h"

namespace Yttrium
{

Allocator *const DefaultAllocator = SystemAllocator::instance();

typedef InstanceGuard<MemoryManager> MemoryManagerGuard;

MemoryManager::MemoryManager()
	: _root_allocator(nullptr)
{
	MemoryManagerGuard::enter(this, S("Duplicate MemoryManager construction"));

	_root_allocator = new(DefaultAllocator->allocate<HeapAllocator>())
		HeapAllocator(DefaultAllocator);

	const_cast<Allocator *&>(DefaultAllocator) = _root_allocator;
}

MemoryManager::~MemoryManager()
{
	const_cast<Allocator *&>(DefaultAllocator) = SystemAllocator::instance();

	DefaultAllocator->delete_(_root_allocator);

	MemoryManagerGuard::leave(this, S("Duplicate MemoryManager construction"));
}

MemoryManager *MemoryManager::instance()
{
	return MemoryManagerGuard::instance;
}

} // namespace Yttrium
