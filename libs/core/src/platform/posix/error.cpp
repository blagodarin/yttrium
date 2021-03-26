// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "error.h"

#include <yttrium/logger.h>

#include <cerrno>
#include <cstdio>
#include <cstring>

namespace Yt
{
	void report_errno(const char* function) noexcept
	{
		const auto error = errno;
		std::array<char, Logger::MaxMessageSize + 1> buffer;
		const auto written = static_cast<size_t>(std::snprintf(buffer.data(), buffer.size(), "[%s] (%d) ", function, error));
		if (written < Logger::MaxMessageSize)
		{
			const auto status = ::strerror_r(error, buffer.data() + written, buffer.size() - written);
#if (_POSIX_C_SOURCE >= 200112L) && !_GNU_SOURCE
			if (status != 0)
				buffer[written - 2] = '\0'; // Strip ": ".
#else
			if (status != buffer.data() + written)
				std::snprintf(buffer.data() + written, buffer.size() - written, "%s", status);
#endif
		}
		Logger::write(buffer.data());
	}
}
