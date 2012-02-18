#ifndef __APPLICATION_PRIVATE_HPP
#define __APPLICATION_PRIVATE_HPP

#include <Yttrium/application.hpp>
#include <Yttrium/types.hpp>

#include "../file_system/file_system.hpp"
#include "../base/memory/heap_allocator.hpp"
#include "../base/log_manager.hpp"

namespace Yttrium
{

class Application::Private
{
	friend class Application;

public:

	Private()
	{
		_exists = true;
		Yttrium::_heap_allocator = &_heap_allocator;
		Yttrium::_file_system = &_file_system;
	}

	~Private()
	{
		Yttrium::_file_system = nullptr;
		Yttrium::_heap_allocator = nullptr;
		_exists = false;
	}

	void initialize()
	{
	}

	LogManager::Private &log_manager_private()
	{
		return _log_manager_private;
	}

public:

	static bool exists()
	{
		return _exists;
	}

	static Private *pointer()
	{
		return reinterpret_cast<Private *>(_buffer);
	}

private:

	HeapAllocatorImpl   _heap_allocator;
	LogManager::Private _log_manager_private;
	FileSystem          _file_system;

private:

	static bool    _exists;
	static uint8_t _buffer[];
};

} // namespace Yttrium

#endif // __APPLICATION_PRIVATE_HPP
