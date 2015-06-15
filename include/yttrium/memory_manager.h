/// \file
/// \brief Memory manager.

#ifndef __Y_MEMORY_MANAGER_H
#define __Y_MEMORY_MANAGER_H

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

#endif // __Y_MEMORY_MANAGER_H
