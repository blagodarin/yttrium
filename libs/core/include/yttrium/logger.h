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

#pragma once

#include <yttrium/utils/string.h>

#include <functional>
#include <memory>

namespace Yttrium
{
	/// Asynchronous logger.
	class Y_CORE_API Logger
	{
	public:
		/// Maximum message size in bytes.
		static constexpr size_t MaxMessageSize = 255;

		/// Creates a logger that asynchronously feeds messages into the callback
		/// or to the standard error stream if no callback is specified.
		Logger(std::function<void(std::string_view)>&& callback = {});

		/// Destroys the logger.
		/// All pending messages are flushed to the output before the destructor finishes.
		~Logger() noexcept;

		/// Flushes all pending messages to the output.
		static void flush() noexcept;

		/// Writes a message to the log.
		/// The size of the message must not exceed MaxMessageSize bytes.
		static void write(std::string_view) noexcept;

		template <typename... Args>
		static void log(Args&&... args)
		{
			write(make_string(std::forward<Args>(args)...));
		}

	private:
		const std::unique_ptr<class LoggerPrivate> _private;
	};
}
