#include <yttrium/memory_manager.h>

#include <yttrium/proxy_allocator.h>
#include <yttrium/system_allocator.h>

#include "../base/instance_guard.h"
#include "heap_allocator.h"

namespace Yttrium
{

Allocator *const DefaultAllocator = SystemAllocator::instance();

typedef InstanceGuard<MemoryManager> MemoryManagerGuard;

class MemoryManager::Private
{
public:

	Private(MemoryManager *public_, const StaticString &default_name)
		: _instance_guard(public_, "Duplicate MemoryManager construction")
		, _previous_default_allocator(DefaultAllocator)
		, _default_allocator(_previous_default_allocator)
	{
		const_cast<Allocator *&>(DefaultAllocator) =
			Y_NEW(&_default_allocator, ProxyAllocator)(default_name, &_default_allocator);
	}

	~Private()
	{
		Y_DELETE(&_default_allocator, DefaultAllocator);
		const_cast<Allocator *&>(DefaultAllocator) = _previous_default_allocator;
	}

	Allocator *default_allocator()
	{
		return &_default_allocator;
	}

private:

	MemoryManagerGuard  _instance_guard;
	Allocator          *_previous_default_allocator;
	HeapAllocator       _default_allocator;
};

static char _memory_manager_private[sizeof(MemoryManager::Private)];

MemoryManager::MemoryManager(const StaticString &default_name)
	: _private(new(_memory_manager_private) Private(this, default_name))
{
}

MemoryManager::~MemoryManager()
{
	_private->~Private();
}

Allocator *MemoryManager::default_allocator()
{
	return MemoryManagerGuard::instance->_private->default_allocator();
}

} // namespace Yttrium
