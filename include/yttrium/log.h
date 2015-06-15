/// \file
/// \brief Log manager.

#ifndef __Y_LOG_H
#define __Y_LOG_H

#include <yttrium/base.h>
#include <yttrium/string.h>

namespace Yttrium
{
	///
	class Y_API LogManager
	{
	public:

		///
		class Writer
		{
			friend LogManager;

		public:

			///
			~Writer();

		public:

			///
			template <typename T>
			Y_PRIVATE Writer& operator<<(const T& value)
			{
				_message << value;
				return *this;
			}

		private:

			Y_PRIVATE Writer(LogManager* log_manager);

		private:

			LogManager* _log_manager;
			String      _message;
		};

		friend Writer;

	public:

		///
		LogManager(Allocator* allocator = nullptr);

		///
		LogManager(const StaticString& file, Allocator* allocator = nullptr);

		///
		~LogManager();

	public:

		///
		Writer log();

	public:

		///
		static LogManager* instance();

	private:
		Y_MANAGER_PRIVATE(LogManager);
	};
}

///
#define Y_LOG(message) do { Yttrium::LogManager::instance()->log() << message; } while (false)

#endif // __Y_LOG_H
