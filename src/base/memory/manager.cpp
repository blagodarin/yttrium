#include <Yttrium/memory_manager.h>

#include "../instance_guard.h"

#include "heap_allocator.h"

#include <iostream>

namespace Yttrium
{

Allocator *const DefaultAllocator = SystemAllocator::instance();

typedef InstanceGuard<MemoryManager> MemoryManagerGuard;

MemoryManager::MemoryManager()
	: _previous_default_allocator(DefaultAllocator)
{
	MemoryManagerGuard::enter(this, S("Duplicate MemoryManager construction"));

	const_cast<Allocator *&>(DefaultAllocator) =
		Y_NEW(_previous_default_allocator, HeapAllocator)(_previous_default_allocator);
}

MemoryManager::~MemoryManager()
{
	// NOTE: We can't use Y_ASSERT here because it uses logging which has already been shut down,
	// so the only remaining way is to write the message to std::cerr.

	const Allocator::Status &status = DefaultAllocator->status();

	if (status.allocated_blocks)
	{
		std::cerr << "Memory leak detected: " << status.allocated_blocks << " blocks are still allocated!" << std::endl;
	}

	Y_DELETE(_previous_default_allocator, DefaultAllocator);
	const_cast<Allocator *&>(DefaultAllocator) = _previous_default_allocator;

	MemoryManagerGuard::leave(this, S("Duplicate MemoryManager destruction"));
}

MemoryManager *MemoryManager::instance()
{
	return MemoryManagerGuard::instance;
}

} // namespace Yttrium
