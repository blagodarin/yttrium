#include <yttrium/memory_manager.h>

#include <yttrium/proxy_allocator.h>
#include "../base/instance_guard.h"
#include "heap_allocator.h"

#define __Y_CATCH_DEFAULT_ALLOCATOR 0

namespace Yttrium
{
	// The default DefaultAllocator is not nullptr to distinguish it
	// from nullptr specification of a context-default allocator.
	const auto DefaultDefaultAllocator = reinterpret_cast<Allocator*>(-1);

	// Such initialization prevents compiler from placing the "constant" into read-only memory.
	Allocator* const DefaultAllocator = [](){ return DefaultDefaultAllocator; }();

	typedef InstanceGuard<MemoryManager> MemoryManagerGuard;

	class MemoryManager::Private
	{
	public:

		Private(MemoryManager* public_, const StaticString& default_name)
			: _instance_guard(public_, "Duplicate MemoryManager construction")
		{
		#if !__Y_CATCH_DEFAULT_ALLOCATOR
			const_cast<Allocator*&>(DefaultAllocator) =
				Y_NEW(&_default_allocator, ProxyAllocator)(default_name, &_default_allocator);
		#else
			Y_UNUSED(default_name);
		#endif
		}

		~Private()
		{
		#if !__Y_CATCH_DEFAULT_ALLOCATOR
			Y_DELETE(&_default_allocator, DefaultAllocator);
			const_cast<Allocator*&>(DefaultAllocator) = DefaultDefaultAllocator;
		#endif
		}

		Allocator* default_allocator()
		{
			return &_default_allocator;
		}

	private:

		MemoryManagerGuard _instance_guard;
		HeapAllocator      _default_allocator;
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

	Allocator* MemoryManager::default_allocator()
	{
		return MemoryManagerGuard::instance->_private->default_allocator();
	}
}
