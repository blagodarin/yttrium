#include <functional> // greater
#include <map>        // map

#include <Yttrium/log_manager.hpp>
#include <Yttrium/string.hpp>

#include "file.hpp"

namespace Yttrium
{

class LogManager::Private
{
	friend class LogManager;

public:

	Private() throw()
		: _root_level(Logger::None)
	{
	}

	Logger::Level level(const StaticString &name) const throw();

	LogManager log_manager() throw()
	{
		return LogManager(this);
	}

	bool open(const StaticString &file, Logger::OpenMode mode) throw();

	Logger::Level root_level() const throw()
	{
		return _root_level;
	}

	void set_level(const StaticString &name, Logger::Level level);

	void set_root_level(Logger::Level level) throw();

	bool write(const void *buffer, size_t size) throw();

private:

	typedef std::map<String, Logger::Level, std::greater<String> > Levels;

private:

	FileWriterImpl _log;
	Logger::Level  _root_level;
	Levels         _levels;
};

} // namespace Yttrium
