#include <yttrium/log.h>
#include <yttrium/log_manager.h>

#include <yttrium/date_time.h>
#include <yttrium/file.h>
#include <yttrium/memory_manager.h>
#include <yttrium/pointer.h>
#include <yttrium/proxy_allocator.h>
#include "instance_guard.h"

namespace Yttrium
{
	class LogManagerImpl;

	using LogManagerGuard = InstanceGuard<LogManagerImpl>;

	class LogManagerImpl : public LogManager
	{
	public:

		LogManagerImpl(const StaticString& file_name, Allocator* allocator)
			: _allocator("log", allocator)
			, _std_err(File::StdErr, &_allocator)
			, _instance_guard(this, "Duplicate LogManager construction")
		{
			if (!file_name.is_empty())
				_file = File(file_name, File::Write | File::Truncate, &_allocator);
		}

		Allocator* allocator()
		{
			return &_allocator;
		}

		void write(const StaticString& string)
		{
			_std_err.write(string.text(), string.size());
			_file.write(string.text(), string.size());
			_file.flush();
		}

	private:

		ProxyAllocator  _allocator;
		File            _std_err;
		File            _file;
		LogManagerGuard _instance_guard;
	};

	Pointer<LogManager> LogManager::create(const StaticString& file_name, Allocator* allocator)
	{
		if (!allocator)
			allocator = MemoryManager::default_allocator();
		return make_pointer<LogManagerImpl>(*allocator, file_name, allocator);
	}

	Log::Log()
		: _message(64, LogManagerGuard::instance ? LogManagerGuard::instance->allocator() : DefaultAllocator)
	{
		const auto& now = DateTime::now();
		_message
			.append('[')
			.append_dec(now.hour, 2, true)
			.append(':')
			.append_dec(now.minute, 2, true)
			.append(':')
			.append_dec(now.second, 2, true)
			.append(S("] "));
	}

	Log::~Log()
	{
		_message << S("\r\n");
		std::lock_guard<std::mutex> lock(LogManagerGuard::instance_mutex);
		if (LogManagerGuard::instance)
			LogManagerGuard::instance->write(_message);
	}
}
