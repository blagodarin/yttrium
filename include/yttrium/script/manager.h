/// \file
/// \brief Script manager.

#ifndef __Y_SCRIPT_MANAGER_H
#define __Y_SCRIPT_MANAGER_H

#include <yttrium/script/context.h>
#include <yttrium/string.h>

namespace Yttrium
{

/// Script manager.

class Y_API ScriptManager
{
	Y_NONCOPYABLE(ScriptManager);

public:

	///

	ScriptManager(Allocator *allocator = DefaultAllocator) noexcept;

	///

	~ScriptManager() noexcept;

public:

	/// Return the root context reference.

	inline ScriptContext &root_context() noexcept;

public:

	/// Return the global ScriptManager instance.

	static ScriptManager *instance() noexcept;

private:

	ScriptContext _root_context;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ScriptContext &ScriptManager::root_context() noexcept
{
	return _root_context;
}

} // namespace Yttrium

#endif // __Y_SCRIPT_MANAGER_H
