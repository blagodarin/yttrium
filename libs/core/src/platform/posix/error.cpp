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
