/// \file
/// \brief Script manager.

#ifndef _include_yttrium_script_manager_h_
#define _include_yttrium_script_manager_h_

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
