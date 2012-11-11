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
	// TODO: Store the default allocator to restore it in the destructor.

	MemoryManagerGuard::enter(this, S("Duplicate MemoryManager construction"));

	_root_allocator = Y_NEW(DefaultAllocator, HeapAllocator)(DefaultAllocator);

	const_cast<Allocator *&>(DefaultAllocator) = _root_allocator;
}

MemoryManager::~MemoryManager()
{
	// TODO: Add some memory leak detection here.

	const_cast<Allocator *&>(DefaultAllocator) = SystemAllocator::instance();

	Y_DELETE(DefaultAllocator, _root_allocator);

	MemoryManagerGuard::leave(this, S("Duplicate MemoryManager destruction"));
}

MemoryManager *MemoryManager::instance()
{
	return MemoryManagerGuard::instance;
}

} // namespace Yttrium
