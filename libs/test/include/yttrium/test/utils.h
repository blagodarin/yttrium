// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace Yt
{
	class Buffer;
}

Yt::Buffer make_buffer(std::string_view);
Yt::Buffer make_random_buffer(size_t);
