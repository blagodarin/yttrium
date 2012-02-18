/// \file
/// \brief Logger.

#ifndef __Y_LOGGER_HPP
#define __Y_LOGGER_HPP

#include <Yttrium/allocator.hpp>
#include <Yttrium/static_string.hpp>
#include <Yttrium/string.hpp>

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
		friend class Logger;

	public:

		///

		~Writer() noexcept
		{
			_logger._message << _location << Y_S("\r\n");
			_logger.flush();
		}

	public:

		///

		template <typename T>
		Writer &operator <<(const T &value) noexcept
		{
			_logger._message << value;
			return *this;
		}

	private:

		Writer(Logger &logger, Level level, const StaticString &file = StaticString(),
			int line = 0, const StaticString &function = StaticString()) noexcept;

	private:

		Logger &_logger;
		String _location;
	};

	friend class Writer;

public:

	explicit Logger(Allocator *allocator = HeapAllocator::instance()) noexcept
		: _level(root_level())
		, _name(allocator)
		, _message(allocator)
	{
	}

	explicit Logger(Level level, Allocator *allocator = HeapAllocator::instance()) noexcept
		: _level(level)
		, _name(allocator)
		, _message(allocator)
	{
	}

	explicit Logger(const StaticString &name, Allocator *allocator = HeapAllocator::instance()) noexcept
		: _level(level(name))
		, _name(name, allocator)
		, _message(allocator)
	{
	}

	Level level() const noexcept
	{
		return _level;
	}

	Writer log(Level level, const StaticString &file = StaticString(),
		int line = 0, const StaticString &function = StaticString()) noexcept
	{
		return Writer(*this, level, file, line, function);
	}

private:

	static Level level(const StaticString &name) noexcept;

	static Level root_level() noexcept;

private:

	Allocator *allocator() const noexcept
	{
		return _message.allocator();
	}

	void flush() noexcept;

private:

	Level  _level;
	String _name;
	String _message;
};

} // namespace Yttrium

#if !Y_DEBUG
	#define __Y_LOG_PARAMS(level) Yttrium::Logger::level
#else
	#define __Y_LOG_PARAMS(level) Yttrium::Logger::level, Y_S(__FILE__), __LINE__, Y_S(__func__)
#endif

///

#define Y_LOG(logger, message) \
	do \
	{ \
		(logger).log(__Y_LOG_PARAMS(None)) << message; \
	} while (false)

///

#define Y_LOG_IF(logger, condition, message) \
	do \
	{ \
		if (condition) \
		{ \
			(logger).log(__Y_LOG_PARAMS(None)) << message; \
		} \
	} while (false)

///

#define Y_LOG_FATAL(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Fatal) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Fatal)) << message; \
		} \
	} while (false)

///

#define Y_LOG_FATAL_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Fatal && (condition)) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Fatal)) << message; \
		} \
	} while (false)

///

#define Y_LOG_ERROR(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Error) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Error)) << message; \
		} \
	} while (false)

///

#define Y_LOG_ERROR_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Error && (condition)) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Error)) << message; \
		} \
	} while (false)

///

#define Y_LOG_WARNING(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Warning) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Warning)) << message; \
		} \
	} while (false)

///

#define Y_LOG_WARNING_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Warning && (condition)) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Warning)) << message; \
		} \
	} while (false)

///

#define Y_LOG_INFO(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Info) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Info)) << message; \
		} \
	} while (false)

///

#define Y_LOG_INFO_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Info && (condition)) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Info)) << message; \
		} \
	} while (false)

///

#define Y_LOG_DEBUG(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Debug) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Debug)) << message; \
		} \
	} while (false)

///

#define Y_LOG_DEBUG_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Debug && (condition)) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Debug)) << message; \
		} \
	} while (false)

///

#define Y_LOG_TRACE(logger, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Trace) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Trace)) << message; \
		} \
	} while (false)

///

#define Y_LOG_TRACE_IF(logger, condition, message) \
	do \
	{ \
		if ((logger).level() >= Yttrium::Logger::Trace && (condition)) \
		{ \
			(logger).log(__Y_LOG_PARAMS(Trace)) << message; \
		} \
	} while (false)

#endif // __Y_LOGGER_HPP
