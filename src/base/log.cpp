#include <yttrium/log.h>

#include <yttrium/file.h>
#include <yttrium/memory_manager.h>
#include <yttrium/time.h>

#include "instance_guard.h"
#include "../memory/private_allocator.h"

namespace Yttrium
{

typedef InstanceGuard<LogManager> LogManagerGuard;

class Y_PRIVATE LogManager::Private
{
public:

	Private(LogManager* public_, Allocator* allocator)
		: _instance_guard(public_, "Duplicate LogManager construction")
		, _allocator(allocator, "log")
		, _std_err(File::StdErr, _allocator)
	{
	}

	void write(const String& string)
	{
		_std_err.write(string.text(), string.size());
		_file.write(string.text(), string.size());
		_file.flush();
	}

public:

	LogManagerGuard  _instance_guard;
	PrivateAllocator _allocator;
	File             _std_err;
	File             _file;
};

LogManager::Writer::~Writer()
{
	if (_log_manager)
	{
		_message << S("\r\n");
		_log_manager->_private->write(_message);
	}
}

LogManager::Writer::Writer(LogManager* log_manager)
	: _log_manager(log_manager)
	, _message(64, log_manager->_private->_allocator)
{
	DateTime now = DateTime::now();

	_message
		.append('[')
		.append_dec(now.hour, 2, true)
		.append(':')
		.append_dec(now.minute, 2, true)
		.append(':')
		.append_dec(now.second, 2, true)
		.append(S("] "));
}

LogManager::LogManager(Allocator* allocator)
	: _private(nullptr)
{
	if (!allocator)
		allocator = MemoryManager::default_allocator();
	_private = Y_NEW(allocator, LogManager::Private)(this, allocator);
}

LogManager::LogManager(const StaticString& file, Allocator* allocator)
	: LogManager(allocator)
{
	_private->_file = File(file, File::Write | File::Truncate, _private->_allocator);
}

LogManager::~LogManager()
{
	_private->_allocator.delete_private(_private);
}

LogManager::Writer LogManager::log()
{
	return Writer(this);
}

LogManager* LogManager::instance()
{
	return LogManagerGuard::instance;
}

} // namespace Yttrium
