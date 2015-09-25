#include <yttrium/script/manager.h>
#include "manager.h"

#include <yttrium/memory_manager.h>
#include <yttrium/script/context.h>
#include "../base/instance_guard.h"
#include "../memory/private_allocator.h"

namespace Yttrium
{
	Allocator* script_manager_allocator = nullptr;

	using ScriptManagerGuard = InstanceGuard<ScriptManager>;

	class Y_PRIVATE ScriptManager::Private
	{
	public:

		Private(ScriptManager* public_, Allocator* allocator)
			: _allocator(allocator, "script"_s)
			, _root_context(_allocator)
			, _instance_guard(public_, "Duplicate ScriptManager construction")
		{
		}

		static Private* instance()
		{
			return ScriptManagerGuard::instance->_private;
		}

	public:

		PrivateAllocator   _allocator;
		ScriptContext      _root_context;
		ScriptManagerGuard _instance_guard;
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
