/// \file
/// \brief Script manager.

#ifndef __Y_SCRIPT_MANAGER_H
#define __Y_SCRIPT_MANAGER_H

#include <yttrium/global.h>

namespace Yttrium
{
	class Allocator;
	class ScriptContext;

	/// Script manager.
	class Y_API ScriptManager
	{
		friend ScriptContext;

		Y_NONCOPYABLE(ScriptManager);

	public:

		///
		ScriptManager(Allocator* allocator = nullptr);

		///
		~ScriptManager();

	private:

		class Private;

		Private *_private;
	};
}

#endif // __Y_SCRIPT_MANAGER_H
