#ifndef __APPLICATION_PRIVATE_HPP
#define __APPLICATION_PRIVATE_HPP

#include <Yttrium/application.hpp>
#include <Yttrium/types.hpp>

#include "../base/heap_allocator.hpp"
#include "../base/log_manager.hpp"

namespace Yttrium
{

class Application::Private
{
	friend class Application;

public:

	Private() throw()
	{
		_exists = true;
		Yttrium::_heap_allocator = &_heap_allocator;
	}

	~Private() throw()
	{
		Yttrium::_heap_allocator = NULL;
		_exists = false;
	}

	void initialize()
	{
	}

	LogManager &log_manager() throw()
	{
		return _log_manager;
	}

public:

	static bool exists() throw()
	{
		return _exists;
	}

	static Private *pointer() throw()
	{
		return reinterpret_cast<Private *>(_buffer);
	}

private:

	HeapAllocatorImpl _heap_allocator;
	LogManager        _log_manager;

private:

	static bool    _exists;
	static uint8_t _buffer[];
};

} // namespace Yttrium

#endif // __APPLICATION_PRIVATE_HPP
