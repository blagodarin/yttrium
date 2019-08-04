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
#include <cassert>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

namespace
{
	static_assert(Yt::Logger::MaxMessageSize == Yt::RingLog::MaxStringSize);

	std::atomic<bool> _global_logger_created{ false };
	std::atomic<Yt::LoggerPrivate*> _global_logger_private{ nullptr };
}

namespace Yttrium
{
	class LoggerPrivate
	{
	public:
		[[nodiscard]] static std::unique_ptr<LoggerPrivate> create(std::function<void(std::string_view)>&& callback)
		{
			bool expected = false;
			return _global_logger_created.compare_exchange_strong(expected, true)
				? std::make_unique<LoggerPrivate>(std::move(callback))
				: nullptr;
		}

		explicit LoggerPrivate(std::function<void(std::string_view)>&& callback) //-V730
			: _thread{ [this, cb = std::move(callback)] { run(cb ? cb : [](std::string_view message) { std::cerr << message << '\n'; }); } }
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
			_flushed.notify_all();
			_can_process.notify_one();
			_thread.join();
			_global_logger_created = false;
		}

		void flush() noexcept
		{
			std::unique_lock lock{ _mutex };
			++_flushing;
			_flushed.wait(lock, [this] { return (_ring_log.empty() && !_processing) || _stop; });
			--_flushing;
		}

		void push(std::string_view message) noexcept
		{
			{
				std::scoped_lock lock{ _mutex };
				const auto need_notify = _ring_log.empty() && !_processing && !_flushing;
				_ring_log.push(message);
				if (!need_notify)
					return;
			}
			_can_process.notify_one();
		}

	private:
		void run(const std::function<void(std::string_view)>& callback)
		{
			std::string message;
			for (std::unique_lock lock{ _mutex };;)
			{
				_can_process.wait(lock, [this] { return !_ring_log.empty() || _stop; });
				if (_stop && _ring_log.empty())
					break;
				assert(!_ring_log.empty());
				_ring_log.pop(message);
				_processing = true;
				lock.unlock();
				callback(message);
				lock.lock();
				_processing = false;
				if (_flushing > 0 && _ring_log.empty())
				{
					lock.unlock();
					_flushed.notify_all();
					lock.lock();
				}
			}
		}

	private:
		std::mutex _mutex;
		bool _stop = false;
		bool _processing = false;
		size_t _flushing = 0;
		std::condition_variable _can_process;
		std::condition_variable _flushed;
		RingLog _ring_log;
		std::thread _thread;
	};

	Logger::Logger(std::function<void(std::string_view)>&& callback)
		: _private{ LoggerPrivate::create(std::move(callback)) }
	{
	}

	Logger::~Logger() noexcept = default;

	void Logger::flush() noexcept
	{
		if (const auto logger = _global_logger_private.load())
			logger->flush();
	}

	void Logger::write(std::string_view message) noexcept
	{
		if (const auto logger = _global_logger_private.load())
			logger->push(message);
	}
}
