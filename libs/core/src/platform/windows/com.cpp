// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "com.h"

#include <yttrium/exceptions.h>
#include "error.h"

#include <comdef.h>

#include <fmt/format.h>

namespace Yt
{
	ComInitializer::ComInitializer(DWORD coinit)
	{
		const auto hr = ::CoInitializeEx(nullptr, coinit);
		if (FAILED(hr))
			throw InitializationError{ fmt::format("CoInitializeEx failed: {}", error_to_string(hr)) };
	}
}
