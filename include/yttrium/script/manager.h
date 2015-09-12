/// \file
/// \brief Script manager.

#ifndef __Y_SCRIPT_MANAGER_H
#define __Y_SCRIPT_MANAGER_H

#include <yttrium/base.h>
#include <yttrium/global.h>

namespace Yttrium
{
	class Allocator;

	/// Script manager.
	class Y_API ScriptManager
	{
	public:

		///
		ScriptManager(Allocator* allocator = nullptr);

		///
		~ScriptManager();

	private:
		Y_MANAGER_PRIVATE(ScriptManager);
	};
}

#endif
