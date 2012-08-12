#include <Yttrium/script/manager.h>

#include "../base/instance_guard.h"

namespace Yttrium
{

typedef InstanceGuard<ScriptManager> ScriptManagerGuard;

ScriptManager::ScriptManager(Allocator *allocator)
	: _root_context(allocator)
{
	ScriptManagerGuard::enter(this, Y_S("Duplicate ScriptManager construction"));
}

ScriptManager::~ScriptManager()
{
	ScriptManagerGuard::leave(this, Y_S("Duplicate ScriptManager construction"));
}

ScriptManager *ScriptManager::instance()
{
	return ScriptManagerGuard::instance;
}

} // namespace Yttrium
