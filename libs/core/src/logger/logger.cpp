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

#include "ring_log.h"

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

namespace
{
	std::atomic<bool> _global_logger_created{ false };
	std::atomic<Yttrium::LoggerPrivate*> _global_logger_private{ nullptr };
}

namespace Yttrium
{
	class LoggerPrivate
	{
	public:
		[[nodiscard]] static std::unique_ptr<LoggerPrivate> create()
		{
			bool expected = false;
			return _global_logger_created.compare_exchange_strong(expected, true) ? std::make_unique<LoggerPrivate>() : nullptr;
		}

		LoggerPrivate()
		{
			_global_logger_private = this;
		}

		~LoggerPrivate() noexcept
		{
			_global_logger_private = nullptr;
			{
				std::scoped_lock lock{ _mutex };
				_stop = true;
			}
			_ready.notify_one();
			_thread.join();
			_global_logger_created = false;
		}

		void push(std::string_view message) noexcept
		{
			{
				std::scoped_lock lock{ _mutex };
				_ring_log.push(message);
			}
			_ready.notify_one();
		}

	private:
		void run()
		{
			for (std::string message;;)
			{
				{
					std::unique_lock lock{ _mutex };
					_ready.wait(lock, [this] { return !_ring_log.empty() || _stop; });
					if (_stop)
						break;
					_ring_log.pop(message);
				}
				std::cerr << message << '\n';
			}
		}

	private:
		std::mutex _mutex;
		RingLog _ring_log;
		bool _stop = false;
		std::condition_variable _ready;
		std::thread _thread{ [this] { run(); } };
	};

	Logger::Logger()
		: _private{ LoggerPrivate::create() }
	{
	}

	Logger::~Logger() noexcept = default;

	void Logger::flush()
	{
		std::this_thread::sleep_for(std::chrono::seconds{ 1 }); // TODO: Proper flushing.
	}

	void Logger::write(std::string_view message) noexcept
	{
		if (const auto logger = _global_logger_private.load())
			logger->push(message);
	}
}
