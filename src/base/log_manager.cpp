#include <Yttrium/log_manager.h>

#include <Yttrium/allocator.h>

#include "instance_guard.h"

namespace Yttrium
{

typedef InstanceGuard<LogManager> LogManagerGuard;

LogManager::LogManager(const StaticString &file, Logger::OpenMode mode, Allocator *allocator)
	: _allocator(allocator)
	, _file(file, File::Write, allocator)
	, _root_level(Logger::Info)
{
	LogManagerGuard::enter(this, Y_S("Duplicate LogManager construction"));

	if (mode == Logger::Rewrite)
	{
		_file.truncate();
		_file.flush();
	}
}

LogManager::~LogManager()
{
	LogManagerGuard::leave(this, Y_S("Unmatched LogManager destruction"));
}

Logger::Level LogManager::level(const StaticString &name) const
{
	size_t prefix_size = name.find_first('.');

	if (prefix_size == StaticString::End)
	{
		Levels::const_iterator i = _levels.find(String(name, String::Ref));
		return (i != _levels.end() ? i->second : _root_level);
	}

	char prefix_buffer[name.size() + 1]; // NOTE: This might be risky, but generally should be OK.

	prefix_buffer[0] = '\0';

	String prefix(prefix_buffer, 0, String::Ref, nullptr);

	Levels::const_iterator begin = _levels.lower_bound(String(name, String::Ref));
	Levels::const_iterator end   = _levels.upper_bound(prefix.set(name.text(), prefix_size));

	for (Levels::const_iterator i = begin; i != end; ++i)
	{
		if (i->first.size() == name.size())
		{
			if (i->first == name)
			{
				return i->second;
			}
		}
		else if (i->first.size() < name.size())
		{
			prefix.set(name.text(), i->first.size());

			if (i->first == prefix && name[prefix.size()] == '.')
			{
				return i->second;
			}
		}
	}

	return _root_level;
}

void LogManager::set_level(const StaticString &name, Logger::Level level)
{
	_levels.insert(Levels::value_type(String(name, _allocator), level));
}

LogManager *LogManager::instance()
{
	return LogManagerGuard::instance;
}

} // namespace Yttrium
