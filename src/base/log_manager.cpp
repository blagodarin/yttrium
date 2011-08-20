#include <Yttrium/allocators.hpp>

#include "log_manager.hpp"

namespace Yttrium
{

Logger::Level LogManager::level(const StaticString &name) const throw()
{
	size_t prefix_size = name.find_first('.');
	if (prefix_size == StaticString::End)
	{
		Levels::const_iterator i = _levels.find(String(name, String::Reference, NULL));
		return (i != _levels.end() ? i->second : _root_level);
	}

	char prefix_buffer[name.size() + 1]; // NOTE: This might be risky, but generally should be OK.

	prefix_buffer[0] = '\0';

	String prefix(prefix_buffer, 0, String::Reference, NULL);

	Levels::const_iterator begin = _levels.lower_bound(String(name, String::Reference, NULL));
	Levels::const_iterator end = _levels.upper_bound(prefix.set(name.text(), prefix_size));

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

bool LogManager::open(const StaticString &file, Logger::OpenMode mode) throw()
{
	if (_log.open_file(file))
	{
		if (mode == Logger::Rewrite)
		{
			_log.truncate();
			_log.flush();
		}
		return true;
	}
	return false;
}

void LogManager::set_level(const StaticString &name, Logger::Level level)
{
	_levels.insert(Levels::value_type(String(name), level)); // NOTE: No allocator specified!
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
