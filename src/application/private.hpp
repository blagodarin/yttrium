#ifndef __APPLICATION_PRIVATE_HPP
#define __APPLICATION_PRIVATE_HPP

#include <Yttrium/application.hpp>
#include <Yttrium/types.hpp>

#include "../base/memory/heap_allocator.hpp"

namespace Yttrium
{

class Application::Private
{
	friend class Application;

public:

	static Private *pointer()
	{
		return reinterpret_cast<Private *>(_buffer);
	}

private:

	HeapAllocatorImpl _heap_allocator;

private:

	static uint8_t _buffer[];
};

} // namespace Yttrium

#endif // __APPLICATION_PRIVATE_HPP
