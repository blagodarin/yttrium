#ifndef __APPLICATION_PRIVATE_HPP
#define __APPLICATION_PRIVATE_HPP

#include <Yttrium/application.hpp>
#include <Yttrium/types.hpp>

#include "../base/files.hpp"
#include "../base/heap_allocator.hpp"

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
		_heap_allocator.initialize();
	}

	FileWriterImpl *log_file() throw()
	{
		return &_log_file;
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

	FileWriterImpl    _log_file;
	HeapAllocatorImpl _heap_allocator;

private:

	static bool    _exists;
	static uint8_t _buffer[];
};

} // namespace Yttrium

#endif // __APPLICATION_PRIVATE_HPP
