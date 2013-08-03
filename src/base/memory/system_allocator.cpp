#include "system_allocator.h"

#include <atomic>

namespace Yttrium
{

// TODO: Create the SystemAllocator explicitly, in e.g. MemoryManager.

static std::atomic_flag _system_allocator_initialized = ATOMIC_FLAG_INIT;

// The SystemAllocator would never be deleted, but this shouldn't be a problem.

static char _system_allocator_buffer[sizeof(SystemAllocatorImpl)];

SystemAllocator *SystemAllocator::instance()
{
	if (!std::atomic_flag_test_and_set(&_system_allocator_initialized))
	{
		new(_system_allocator_buffer) SystemAllocatorImpl();
	}
	return reinterpret_cast<SystemAllocatorImpl *>(_system_allocator_buffer);
}

} // namespace Yttrium
