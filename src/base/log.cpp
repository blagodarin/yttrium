#include <yttrium/log.h>

#include <yttrium/file.h>
#include <yttrium/time.h>

#include "instance_guard.h"
#include "memory/private_allocator.h"

namespace Yttrium
{

typedef InstanceGuard<LogManager> LogManagerGuard;

class LogManager::Private
{
public:

	Private(LogManager *public_, const StaticString &file, Allocator *allocator)
		: _instance_guard(public_, "Duplicate LogManager construction")
		, _allocator(allocator, "log")
		, _file(file, File::Write | File::Truncate, _allocator)
	{
	}

public:

	LogManagerGuard  _instance_guard;
	PrivateAllocator _allocator;
	File             _file;
};

LogManager::Writer::~Writer()
{
	if (_log_manager)
	{
		_message << S("\r\n");
		_log_manager->_private->_file.write(_message.text(), _message.size());
		_log_manager->_private->_file.flush();
	}
}

LogManager::Writer::Writer(LogManager *log_manager)
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

LogManager::LogManager(const StaticString &file, Allocator *allocator)
	: _private(Y_NEW(allocator, LogManager::Private)(this, file, allocator))
{
}

LogManager::~LogManager()
{
	_private->_allocator.delete_private(_private);
}

LogManager::Writer LogManager::log()
{
	return Writer(this);
}

LogManager *LogManager::instance()
{
	return LogManagerGuard::instance;
}

} // namespace Yttrium
