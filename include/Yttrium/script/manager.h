/// \file
/// \brief Script manager.

#ifndef __Y_SCRIPT_MANAGER_H
#define __Y_SCRIPT_MANAGER_H

#include <Yttrium/noncopyable.h>
#include <Yttrium/script/context.h>
#include <Yttrium/string.h>

namespace Yttrium
{

/// Script manager.

class Y_API ScriptManager: public Noncopyable
{
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
