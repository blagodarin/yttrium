/// \file
/// \brief Memory manager.

#ifndef __Y_MEMORY_MANAGER_HPP
#define __Y_MEMORY_MANAGER_HPP

#include <Yttrium/noncopyable.hpp>

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

	Allocator *root_allocator() noexcept
	{
		return _root_allocator;
	}

public:

	///

	static MemoryManager *instance() noexcept;

private:

	Allocator *_root_allocator;
};

} // namespace Yttrium

#endif // __Y_MEMORY_MANAGER_HPP
