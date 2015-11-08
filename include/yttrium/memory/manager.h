/// \file
/// \brief Memory manager.

#ifndef _include_yttrium_memory_manager_h_
#define _include_yttrium_memory_manager_h_

#include <yttrium/base.h>
#include <yttrium/static_string.h>

namespace Yttrium
{
	class MemoryManagerPrivate;

	///
	class Y_API MemoryManager
	{
	public:

		///
		MemoryManager(const StaticString& default_name = "default"_s);

		///
		~MemoryManager();

		///
		static Allocator* default_allocator();

		MemoryManager(const MemoryManager&) = delete;
		MemoryManager(MemoryManager&&) = delete;
		MemoryManager& operator=(const MemoryManager&) = delete;
		MemoryManager& operator=(MemoryManager&&) = delete;
	private:
		MemoryManagerPrivate* const _private;
	};
}

#endif
