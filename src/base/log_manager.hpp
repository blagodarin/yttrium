#include <functional> // greater
#include <map>        // map

#include <Yttrium/log_manager.hpp>
#include <Yttrium/string.hpp>

#include "file.hpp"

namespace Yttrium
{

extern LogManager::Private *_log_manager_private;

class LogManager::Private
{
	friend class LogManager;

public:

	Private()
		: _root_level(Logger::None)
	{
		_log_manager_private = this;
	}

	~Private()
	{
		_log_manager_private = nullptr;
	}

public:

	Logger::Level level(const StaticString &name) const;

	LogManager log_manager()
	{
		return LogManager(this);
	}

	bool open(const StaticString &file, Logger::OpenMode mode);

	Logger::Level root_level() const
	{
		return _root_level;
	}

	void set_level(const StaticString &name, Logger::Level level);

	void set_root_level(Logger::Level level);

	bool write(const void *buffer, size_t size);

private:

	typedef std::map<String, Logger::Level, std::greater<String> > Levels;

private:

	StaticFile    _log;
	Logger::Level _root_level;
	Levels        _levels;
};

} // namespace Yttrium
