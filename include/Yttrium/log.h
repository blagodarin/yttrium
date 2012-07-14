/// \file
/// \brief Log manager.

#ifndef __Y_LOG_H
#define __Y_LOG_H

#include <Yttrium/file.h>
#include <Yttrium/noncopyable.h>
#include <Yttrium/string.h>

namespace Yttrium
{

///

class Y_API LogManager: public Noncopyable
{
public:

	///

	class Writer
	{
		friend class LogManager;

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

		Writer(LogManager *log_manager) noexcept;

	private:

		LogManager *_log_manager;
		String      _message;
	};

	friend class Writer;

public:

	///

	LogManager(const StaticString &file, Allocator *allocator = DefaultAllocator) noexcept;

	///

	~LogManager() noexcept;

public:

	///

	Writer log() noexcept
	{
		return Writer(this);
	}

public:

	///

	static LogManager *instance() noexcept;

private:

	Allocator *_allocator;
	File       _file;
};

} // namespace Yttrium

///

#define Y_LOG(message) do { Yttrium::LogManager::instance()->log() << message; } while (false)

#endif // __Y_LOG_H
