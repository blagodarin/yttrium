#include <yttrium/log.h>

#include "log.h"

#include <iostream>

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace Yttrium
{
	LogManager::LogManager(Allocator& allocator)
		: _allocator(allocator)
	{
	}

	void LogManager::set_file(const StaticString& name)
	{
		_writer = Writer(name);
	}

	void LogManager::write(const StaticString& string)
	{
		std::cerr.write(string.text(), string.size());
		_writer.write_all(string);
	}

	void Log::set_file(const StaticString& name)
	{
		_log_manager.set_file(name);
	}

	Log::Log()
		: _message(64, &_log_manager.allocator())
	{
		const auto now = boost::posix_time::second_clock::local_time().time_of_day();
		_message << '[' << dec(now.hours(), 2) << ':' << dec(now.minutes(), 2) << ':' << dec(now.seconds(), 2) << "] "_s;
	}

	Log::~Log()
	{
		_message << "\r\n"_s;
		_log_manager.write(_message);
	}
}
