//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/logger.h>

#include "utils/ring_log.h"

#include <atomic>
#include <iostream>
#include <thread>

namespace
{
	std::atomic<Yttrium::Logger*> _the_logger{ nullptr };
}

namespace Yttrium
{
	class LoggerPrivate
	{
	public:
		static std::unique_ptr<LoggerPrivate> create(Logger* logger)
		{
			// A race condition can occur if one thread is creating a logger and another thread is
			// trying to access it. Since it is impossible in a practical scenario, we don't care.
			Logger* expected = nullptr;
			return _the_logger.compare_exchange_strong(expected, logger) ? std::make_unique<LoggerPrivate>() : nullptr;
		}

		~LoggerPrivate() noexcept
		{
			if (_ring_log.shutdown())
				_thread.join();
			_the_logger = nullptr;
		}

	private:
		void run()
		{
			std::string string;
			while (_ring_log.pop(string, true))
				std::cerr << string << '\n';
		}

	private:
		std::thread _thread{ [this] { run(); } };
	};

	Logger::Logger()
		: _private{ LoggerPrivate::create(this) }
	{
	}

	Logger::~Logger() noexcept = default;

	void Logger::flush()
	{
		std::this_thread::sleep_for(std::chrono::seconds{ 1 }); // TODO: Proper flushing.
	}

	void Logger::write(std::string_view message) noexcept
	{
		_ring_log.push(message);
	}
}
