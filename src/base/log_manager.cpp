#include <Yttrium/allocators.hpp>

#include "log_manager.hpp"

namespace Yttrium
{

Logger::Level LogManager::level(const StaticString &name, Allocator *allocator) const throw()
{
	size_t prefix_size = name.find_first('.');
	if (prefix_size == StaticString::End)
	{
		Levels::const_iterator i = _levels.find(String(name, String::Reference, allocator));
		return (i != _levels.end() ? i->second : _root_level);
	}

	String prefix(name.size(), allocator); // NOTE: This seems to be the only allocation. Perhaps it should be done by the StackAllocator?

	Levels::const_iterator begin = _levels.lower_bound(String(name, String::Reference, allocator));
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
