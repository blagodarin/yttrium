/// \file
/// \brief Log manager.

#ifndef __Y_LOG_H
#define __Y_LOG_H

#include <yttrium/string.h>

namespace Yttrium
{

///

class Y_API LogManager
{
	Y_NONCOPYABLE(LogManager);

public:

	///

	class Writer
	{
		friend LogManager;

	public:

		///

		~Writer() noexcept;

	public:

		///

		template <typename T>
		Y_PRIVATE Writer &operator <<(const T &value) noexcept
		{
			_message << value;
			return *this;
		}

	private:

		Y_PRIVATE Writer(LogManager *log_manager) noexcept;

	private:

		LogManager *_log_manager;
		String      _message;
	};

	friend Writer;

public:

	///

	LogManager(const StaticString &file, Allocator *allocator = DefaultAllocator) noexcept;

	///

	~LogManager() noexcept;

public:

	///

	Writer log() noexcept;

public:

	///

	static LogManager *instance() noexcept;

private:

	class Private;

	Private *_private;
};

} // namespace Yttrium

///

#define Y_LOG(message) do { Yttrium::LogManager::instance()->log() << message; } while (false)

///

#define Y_LOG_IF(condition, message) do { if (condition) Yttrium::LogManager::instance()->log() << message; } while (false)

#endif // __Y_LOG_H
