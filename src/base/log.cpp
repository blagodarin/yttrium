#include <yttrium/log.h>

#include <yttrium/time.h>
#include "log.h"

namespace Yttrium
{
	LogManager::LogManager(Allocator& allocator)
		: _allocator(allocator)
	{
	}

	void LogManager::set_file(const StaticString& name)
	{
		_file = File(name, File::Write | File::Truncate);
	}

	void LogManager::write(const StaticString& string)
	{
		if (!_std_err)
			_std_err = File(File::StdErr);
		_std_err.write(string.text(), string.size());
		_file.write(string.text(), string.size());
	}

	void Log::set_file(const StaticString& name)
	{
		_log_manager.set_file(name);
	}

	Log::Log()
		: _message(64, &_log_manager.allocator())
	{
		const auto& now = DateTime::now();
		_message << '[' << dec(now.hour, 2) << ':' << dec(now.minute, 2) << ':' << dec(now.second, 2) << "] "_s;
	}

	Log::~Log()
	{
		_message << "\r\n"_s;
		_log_manager.write(_message);
	}
}
