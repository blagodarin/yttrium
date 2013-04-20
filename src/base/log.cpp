#include <Yttrium/log.h>

#include <Yttrium/time.h>

#include "instance_guard.h"

namespace Yttrium
{

typedef InstanceGuard<LogManager> LogManagerGuard;

LogManager::Writer::~Writer()
{
	if (_log_manager)
	{
		_message << S("\r\n");
		_log_manager->_file.write(_message.text(), _message.size());
		_log_manager->_file.flush();
	}
}

LogManager::Writer::Writer(LogManager *log_manager)
	: _log_manager(log_manager)
	, _message(64, log_manager->_allocator)
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
	: _allocator(allocator)
	, _file(file, File::Write | File::Truncate, allocator)
{
	LogManagerGuard::enter(this, "Duplicate LogManager construction");
}

LogManager::~LogManager()
{
	LogManagerGuard::leave(this, "Unmatched LogManager destruction");
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
