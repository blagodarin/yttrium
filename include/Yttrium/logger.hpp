/// \file
/// \brief Logger.

#ifndef __Y_LOGGER_HPP
#define __Y_LOGGER_HPP

#include <Yttrium/allocators.hpp>
#include <Yttrium/static_string.hpp>

namespace Yttrium
{

/// Logger.

class Y_API Logger
{
public:

	/// Logging levels.

	enum Level
	{
		None,    ///< Disables all levels.
		Fatal,   ///< %Fatal level.
		Error,   ///< %Error level.
		Warning, ///< %Warning level.
		Info,    ///< %Informational level.
		Debug,   ///< %Debug level.
		Trace,   ///< %Trace level.
		All,     ///< Enables all levels.
	};

	///

	enum OpenMode
	{
		Rewrite, ///< %Rewrite the log.
		Append,  ///< %Append to the existing log.
	};

	///

	class Writer
	{
	public:

		///

		Writer(Logger &logger);

	public:

		Writer& operator <<(const StaticString &value);
	};

	friend class Writer;

public:

	Logger(Allocator *allocator = HeapAllocator::instance());

	Logger(const StaticString &name, Allocator *allocator = HeapAllocator::instance());

	Logger(Level level, Allocator *allocator = HeapAllocator::instance());

	Writer log() throw();

	Writer log(const StaticString &file, int line) throw();

	Writer log(const StaticString &file, int line, const StaticString &function) throw();

public:

	///
	/// \note This function is not reentrant!

	static bool open(const StaticString &file, OpenMode mode = Append, Level root_level = Info);

	///
	/// \note This function is not reentrant!

	static void set_root_level(Level level);

	///
	/// \note This function is not reentrant!

	static void set_level(const StaticString &name, Level level);
};

} // namespace Yttrium

#ifndef __Y_DEBUG
	#define __Y_LOCATION
#else
	#define __Y_LOCATION __FILE__, __LINE__, __func__
#endif

///

#define Y_LOG(logger, message) \
	do \
	{ \
		(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_LOG_IF(logger, condition, message) \
	do \
	{ \
		if (condition) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_FATAL(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Fatal) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_FATAL_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Fatal && (condition)) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_ERROR(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Error) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_ERROR_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Error && (condition)) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_WARNING(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Warning) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_WARNING_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Warning && (condition)) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_INFO(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Info) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_INFO_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Info && (condition)) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_DEBUG(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Debug) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_DEBUG_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Debug && (condition)) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_TRACE(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Trace) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

///

#define Y_TRACE_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Trace && (condition)) \
			(logger).log(__Y_LOCATION) << (message); \
	} while (false)

#endif // __Y_LOGGER_HPP
