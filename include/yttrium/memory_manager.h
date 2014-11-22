/// \file
/// \brief Memory manager.

#ifndef __Y_MEMORY_MANAGER_H
#define __Y_MEMORY_MANAGER_H

#include <yttrium/static_string.h>

namespace Yttrium
{

///
class Y_API MemoryManager
{
	Y_NONCOPYABLE(MemoryManager);

public:

	///
	MemoryManager(const StaticString& default_name = S("default")) noexcept;

	///
	~MemoryManager() noexcept;

public:

	///
	static Allocator *default_allocator() noexcept;

public:

	class Private;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_MEMORY_MANAGER_H
