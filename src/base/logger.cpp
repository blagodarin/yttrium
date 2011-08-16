#include <Yttrium/files.hpp>
#include <Yttrium/logger.hpp>

namespace Yttrium
{

Logger::Writer::Writer(Logger &logger, Level level, const StaticString &file, int line, const StaticString &function)
	: _logger(logger)
	, _message(128, logger.allocator())
	, _location(64, logger.allocator())
{
	if (file)
	{
		_location << " {" << file << ':' << line;
		if (function)
		{
			_location << ' ' << function;
		}
		_location << '}';
	}
	_location << "\r\n";
}

// NOTE: The log file is never closed once opened, but this shouldn't be an issue.
// NOTE: Or maybe it should be moved under the Application control.

static FileWriter *_log_file = NULL;

bool Logger::open(const StaticString &file, OpenMode mode, Level root_level)
{
	return false;
}

void Logger::set_root_level(Level level) throw()
{
}

void Logger::set_level(const StaticString &name, Level level)
{
}

void Logger::write(const StaticString& message)
{
}

Logger::Level Logger::root_level() throw()
{
	return None;
}

Logger::Level Logger::level(const StaticString &name)
{
	return None;
}

} // namespace Yttrium
