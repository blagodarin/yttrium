#include <Yttrium/logger.hpp>

#include <Yttrium/file.hpp>
#include <Yttrium/log_manager.hpp>
#include <Yttrium/time.hpp>

namespace Yttrium
{

Logger::Writer::Writer(Logger &logger, Level level, const StaticString &file, int line, const StaticString &function)
	: _logger(logger)
	, _location(64, logger.allocator())
{
	DateTime now = DateTime::now();

	_logger._message
		.append('[')
		.append_dec(now.hour, 2, true)
		.append(':')
		.append_dec(now.minute, 2, true)
		.append(':')
		.append_dec(now.second, 2, true)
		.append(Y_S("] "));

	if (!_logger._name.is_empty())
	{
		_logger._message << _logger._name << " - ";
	}

	switch (level)
	{
	case Fatal:   _logger._message << Y_S("fatal: ");   break;
	case Error:   _logger._message << Y_S("error: ");   break;
	case Warning: _logger._message << Y_S("warning: "); break;
	case Info:    _logger._message << Y_S("info: ");    break;
	case Debug:   _logger._message << Y_S("debug: ");   break;
	case Trace:   _logger._message << Y_S("trace: ");   break;
	default:                                            break;
	}

	if (!file.is_empty())
	{
		_location << Y_S(" {") << file << ':' << line;
		if (!function.is_empty())
		{
			_location << Y_S(" - ") << function;
		}
		_location << '}';
	}
}

Logger::Logger(Allocator *allocator)
	: _level(None)
	, _name(allocator)
	, _message(allocator)
{
	LogManager *manager = LogManager::instance();

	if (manager)
	{
		_level = manager->root_level();
	}
}

Logger::Logger(const StaticString &name, Allocator *allocator)
	: _level(None)
	, _name(name, allocator)
	, _message(allocator)
{
	LogManager *manager = LogManager::instance();

	if (manager)
	{
		_level = manager->level(name);
	}
}

void Logger::flush()
{
	LogManager *manager = LogManager::instance();

	if (manager)
	{
		manager->_file.write(_message.text(), _message.size());
		manager->_file.flush();
		_message.clear();
	}
}

} // namespace Yttrium
