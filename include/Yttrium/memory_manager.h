/// \file
/// \brief Memory manager.

#ifndef __Y_MEMORY_MANAGER_H
#define __Y_MEMORY_MANAGER_H

#include <Yttrium/noncopyable.h>

namespace Yttrium
{

class Allocator;

///

class Y_API MemoryManager: public Noncopyable
{
public:

	///

	MemoryManager() noexcept;

	///

	~MemoryManager() noexcept;

public:

	///

	inline Allocator *root_allocator() noexcept;

public:

	///

	static MemoryManager *instance() noexcept;

private:

	Allocator *_root_allocator;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Allocator *MemoryManager::root_allocator() noexcept
{
	return _root_allocator;
}

} // namespace Yttrium

#endif // __Y_MEMORY_MANAGER_H
