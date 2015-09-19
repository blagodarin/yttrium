/// \file
/// \brief Memory manager.

#ifndef _include_yttrium_memory_manager_h_
#define _include_yttrium_memory_manager_h_

#include <yttrium/base.h>
#include <yttrium/static_string.h>

namespace Yttrium
{
	///
	class Y_API MemoryManager
	{
	public:

		///
		MemoryManager(const StaticString& default_name = S("default"));

		///
		~MemoryManager();

		///
		static Allocator* default_allocator();

	private:
		Y_MANAGER_PRIVATE(MemoryManager);
	};
}

#endif
