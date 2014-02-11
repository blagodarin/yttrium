#include <yttrium/script/manager.h>

#include <yttrium/memory_manager.h>
#include <yttrium/script/context.h>

#include "../base/instance_guard.h"
#include "../base/memory/private_allocator.h"

namespace Yttrium
{

typedef InstanceGuard<ScriptManager> ScriptManagerGuard;

class Y_PRIVATE ScriptManager::Private
{
public:

	Private(ScriptManager *public_, Allocator *allocator)
		: _instance_guard(public_, "Duplicate ScriptManager construction")
		, _allocator(allocator, "script")
		, _root_context(_allocator)
	{
	}

public:

	ScriptManagerGuard _instance_guard;
	PrivateAllocator   _allocator;
	ScriptContext      _root_context;
};

ScriptManager::ScriptManager(Allocator *allocator)
	: _private(nullptr)
{
	if (!allocator)
		allocator = MemoryManager::default_allocator();
	_private = Y_NEW(allocator, ScriptManager::Private)(this, allocator);
}

ScriptManager::~ScriptManager()
{
	_private->_allocator.delete_private(_private);
}

ScriptContext &ScriptContext::global()
{
	return ScriptManagerGuard::instance->_private->_root_context;
}

} // namespace Yttrium
