// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <functional>
#include <memory>
#include <string_view>

namespace Yt
{
	/// Asynchronous logger.
	class Logger
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

	private:
		const std::unique_ptr<class LoggerPrivate> _private;
	};
}
