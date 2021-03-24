// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <functional>

namespace Yt
{
	class Bgra32;
	class Buffer;

	Buffer make_bgra32_tga(size_t width, size_t height, const std::function<Yt::Bgra32(size_t, size_t)>& callback);
}
