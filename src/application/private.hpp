#ifndef __APPLICATION_PRIVATE_HPP
#define __APPLICATION_PRIVATE_HPP

#include <Yttrium/application.hpp>
#include <Yttrium/types.hpp>

#include "../file_system/file_system.hpp"
#include "../base/memory/heap_allocator.hpp"
#include "../base/log_manager.hpp"
#include "../audio/manager.hpp"

namespace Yttrium
{

class Application::Private
{
	friend class Application;

public:

	LogManager::Private &log_manager_private()
	{
		return _log_manager_private;
	}

public:

	static Private *pointer()
	{
		return reinterpret_cast<Private *>(_buffer);
	}

private:

	HeapAllocatorImpl   _heap_allocator;
	FileSystem          _file_system;
	LogManager::Private _log_manager_private;

private:

	static uint8_t _buffer[];
};

} // namespace Yttrium

#endif // __APPLICATION_PRIVATE_HPP
