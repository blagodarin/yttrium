/// \file
/// \brief Log manager.

#ifndef __Y_LOG_MANAGER_HPP
#define __Y_LOG_MANAGER_HPP

#include <Yttrium/logger.hpp>
#include <Yttrium/static_string.hpp>

namespace Yttrium
{

///

class Y_API LogManager
{
public:

	///

	Logger::Level level(const StaticString &name) const noexcept;

	///

	bool open(const StaticString &file, Logger::OpenMode mode, Logger::Level root_level = Logger::Info) noexcept;

	///

	Logger::Level root_level() const noexcept;

	///

	void set_level(const StaticString &name, Logger::Level level) noexcept;

	///

	void set_root_level(Logger::Level level) noexcept;

public:

	class Private;

	friend class Private;

private:

	LogManager(Private *private_);

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_LOG_MANAGER_HPP
