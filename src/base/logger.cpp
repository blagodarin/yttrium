#include <Yttrium/files.hpp>
#include <Yttrium/logger.hpp>
#include <Yttrium/time.hpp>

#include "../application/private.hpp"

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
		.append("] ");

	if (_logger._name)
	{
		_logger._message << _logger._name << " - ";
	}

	switch (level)
	{
	case Fatal:   _logger._message << "fatal: ";   break;
	case Error:   _logger._message << "error: ";   break;
	case Warning: _logger._message << "warning: "; break;
	case Info:    _logger._message << "info: ";    break;
	case Debug:   _logger._message << "debug: ";   break;
	case Trace:   _logger._message << "trace: ";   break;
	default:                                       break;
	}

	if (file)
	{
		_location << " {" << file << ':' << line;
		if (function)
		{
			_location << "::" << function;
		}
		_location << '}';
	}
}

Logger::Level Logger::level(const StaticString &name) throw()
{
	if (Application::Private::exists())
	{
		return Application::Private::pointer()->log_manager_private().level(name);
	}
	return None;
}

Logger::Level Logger::root_level() throw()
{
	if (Application::Private::exists())
	{
		return Application::Private::pointer()->log_manager_private().root_level();
	}
	return None;
}

void Logger::flush() throw()
{
	if (Application::Private::exists())
	{
		if (Application::Private::pointer()->log_manager_private().write(_message.text(), _message.size()))
		{
			_message.clear();
		}
	}
}

} // namespace Yttrium
