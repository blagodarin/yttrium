#include <yttrium/global.h>

#if Y_IS_POSIX
	#include "posix/system_allocator.h"
#elif Y_IS_WINDOWS
	#include "windows/system_allocator.h"
#endif

#include <yttrium/assert.h>

#include <atomic>

namespace Yttrium
{

size_t SystemAllocator::upper_bound(size_t size)
{
	return instance()->_private->total_size(size) - SystemAllocator::Private::ReservedSize;
}

static std::atomic_flag _system_allocator_initialized = ATOMIC_FLAG_INIT;
static char _system_allocator_private[sizeof(SystemAllocator::Private)];
static char _system_allocator[sizeof(SystemAllocator)];

SystemAllocator *SystemAllocator::instance()
{
	if (!std::atomic_flag_test_and_set(&_system_allocator_initialized))
	{
		new(_system_allocator_private) SystemAllocator::Private();
		new(_system_allocator) SystemAllocator();
	}

	return reinterpret_cast<SystemAllocator *>(_system_allocator);
}

SystemAllocator::SystemAllocator()
	: _private(reinterpret_cast<SystemAllocator::Private *>(_system_allocator_private))
{
}

SystemAllocator::~SystemAllocator()
{
	Y_ASSERT(false);
}

} // namespace Yttrium
