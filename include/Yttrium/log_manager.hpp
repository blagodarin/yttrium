/// \file
/// \brief Log manager.

#ifndef __Y_LOG_MANAGER_HPP
#define __Y_LOG_MANAGER_HPP

#include <functional> // std::greater
#include <map>        // std::map

#include <Yttrium/allocator.hpp>
#include <Yttrium/file.hpp>
#include <Yttrium/logger.hpp>
#include <Yttrium/noncopyable.hpp>
#include <Yttrium/static_string.hpp>

namespace Yttrium
{

///

class Y_API LogManager: public Noncopyable
{
	friend class Logger;

public:

	///

	LogManager(const StaticString &file, Logger::OpenMode mode, Allocator *allocator = DefaultAllocator) noexcept;

	///

	~LogManager() noexcept;

public:

	///

	Logger::Level level(const StaticString &name) const noexcept;

	///

	Logger::Level root_level() const noexcept
	{
		return _root_level;
	}

	///

	void set_level(const StaticString &name, Logger::Level level) noexcept;

	///

	void set_root_level(Logger::Level level) noexcept
	{
		_root_level = level;
	}

public:

	///

	static LogManager *instance() noexcept;

private:

	typedef std::map<String, Logger::Level, std::greater<String>> Levels;

private:

	Allocator     *_allocator;
	File           _file;
	Logger::Level  _root_level;
	Levels         _levels;
};

} // namespace Yttrium

#endif // __Y_LOG_MANAGER_HPP
