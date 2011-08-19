#include <Yttrium/logger.hpp>
#include <Yttrium/static_string.hpp>

#include "files.hpp"

namespace Yttrium
{

class LogManager
{
public:

	LogManager() throw()
		: _root_level(Logger::None)
	{
	}

	Logger::Level level(const StaticString &name) const throw();

	bool open(const StaticString &file, Logger::OpenMode mode) throw();

	Logger::Level root_level() const throw()
	{
		return _root_level;
	}

	void set_level(const StaticString &name, Logger::Level level);

	void set_root_level(Logger::Level level) throw();

	bool write(const void *buffer, size_t size) throw();

private:

	FileWriterImpl _log;
	Logger::Level  _root_level;
};

} // namespace Yttrium
