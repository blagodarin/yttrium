// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/image/image.h>

#include <cstring>

namespace Yt
{
	inline bool operator==(const Image& a, const Image& b) noexcept
	{
		const auto info = a.info();
		if (info != b.info())
			return false;
		for (std::size_t y = 0; y < info.height(); ++y)
		{
			const auto aRow = static_cast<const std::uint8_t*>(a.data()) + y * info.stride();
			const auto bRow = static_cast<const std::uint8_t*>(b.data()) + y * info.stride();
			if (std::memcmp(aRow, bRow, info.width() * ImageInfo::pixel_size(info.pixel_format())))
				return false;
		}
		return true;
	}
}
