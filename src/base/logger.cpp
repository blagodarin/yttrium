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

Logger::Level Logger::level(const StaticString &name)
{
	if (Application::Private::exists())
	{
		return Application::Private::pointer()->log_manager().level(name);
	}
	return None;
}

bool Logger::open(const StaticString &file, OpenMode mode, Level root_level)
{
	if (Application::Private::exists())
	{
		LogManager &log_manager = Application::Private::pointer()->log_manager();

		if (log_manager.open(file, mode))
		{
			log_manager.set_root_level(root_level);
			return true;
		}
	}
	return false;
}

Logger::Level Logger::root_level() throw()
{
	if (Application::Private::exists())
	{
		return Application::Private::pointer()->log_manager().root_level();
	}
	return None;
}

void Logger::set_level(const StaticString &name, Level level)
{
	if (Application::Private::exists())
	{
		Application::Private::pointer()->log_manager().set_level(name, level);
	}
}

void Logger::set_root_level(Level level) throw()
{
	if (Application::Private::exists())
	{
		Application::Private::pointer()->log_manager().set_root_level(level);
	}
}

void Logger::flush() throw()
{
	if (Application::Private::exists())
	{
		if (Application::Private::pointer()->log_manager().write(_message.text(), _message.size()))
		{
			_message.clear();
		}
	}
}

} // namespace Yttrium
