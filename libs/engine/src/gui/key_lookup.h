// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/key.h>

#include <string_view>

namespace Yt
{
	Y_ENGINE_API Key lookup_key(std::string_view name);
}
