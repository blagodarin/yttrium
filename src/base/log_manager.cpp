#include "log_manager.hpp"

namespace Yttrium
{

Logger::Level LogManager::level(const StaticString &name) const throw()
{
	return _root_level; // This is the best stub for now.
}

bool LogManager::open(const StaticString &file, Logger::OpenMode mode) throw()
{
	if (_log.open_file(file))
	{
		if (mode == Logger::Rewrite)
		{
			_log.truncate();
			_log.flush(); // NOTE: Do we need this?
		}
		return true;
	}
	return false;
}

void LogManager::set_level(const StaticString &name, Logger::Level level)
{
}

void LogManager::set_root_level(Logger::Level level) throw()
{
	_root_level = level;
}

bool LogManager::write(const void *buffer, size_t size) throw()
{
	if (_log.write(buffer, size))
	{
		_log.flush();
		return true;
	}
	return false;
}

} // namespace Yttrium
