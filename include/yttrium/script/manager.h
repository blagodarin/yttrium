/// \file
/// \brief Script manager.

#ifndef __Y_SCRIPT_MANAGER_H
#define __Y_SCRIPT_MANAGER_H

#include <yttrium/types.h>

namespace Yttrium
{

class ScriptContext;

/// Script manager.

class Y_API ScriptManager
{
	friend ScriptContext;

	Y_NONCOPYABLE(ScriptManager);

public:

	///

	ScriptManager(Allocator *allocator = DefaultAllocator) noexcept;

	///

	~ScriptManager() noexcept;

private:

	class Private;

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_SCRIPT_MANAGER_H
