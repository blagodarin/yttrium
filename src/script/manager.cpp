#include <yttrium/script/manager.h>
#include "manager.h"

#include <yttrium/memory_manager.h>
#include <yttrium/script/context.h>
#include "../base/instance_guard.h"
#include "../memory/private_allocator.h"

namespace Yttrium
{
	Allocator* script_manager_allocator = nullptr;

	typedef InstanceGuard<ScriptManager> ScriptManagerGuard;

	class Y_PRIVATE ScriptManager::Private
	{
	public:

		Private(ScriptManager* public_, Allocator* allocator)
			: _instance_guard(public_, "Duplicate ScriptManager construction")
			, _allocator(allocator, "script")
			, _root_context(_allocator)
		{
		}

		static Private* instance()
		{
			return ScriptManagerGuard::instance->_private;
		}

	public:

		ScriptManagerGuard _instance_guard;
		PrivateAllocator   _allocator;
		ScriptContext      _root_context;
	};

	ScriptManager::ScriptManager(Allocator* allocator)
	{
		if (!allocator)
			allocator = MemoryManager::default_allocator();
		_private = Y_NEW(allocator, ScriptManager::Private)(this, allocator);
		script_manager_allocator = _private->_allocator;
	}

	ScriptManager::~ScriptManager()
	{
		script_manager_allocator = nullptr;
		_private->_allocator.delete_private(_private);
	}

	ScriptContext& ScriptContext::global()
	{
		return ScriptManager::Private::instance()->_root_context;
	}
}
