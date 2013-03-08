/// \file
/// \brief Memory manager.

#ifndef __Y_MEMORY_MANAGER_H
#define __Y_MEMORY_MANAGER_H

#include <Yttrium/global.h>

namespace Yttrium
{

class Allocator;

///

class Y_API MemoryManager
{
	Y_NONCOPYABLE(MemoryManager);

public:

	///

	MemoryManager() noexcept;

	///

	~MemoryManager() noexcept;

public:

	///

	static Allocator *default_allocator() noexcept;

private:

	Allocator *_previous_default_allocator;
	Allocator *_default_allocator;
};

} // namespace Yttrium

#endif // __Y_MEMORY_MANAGER_H
