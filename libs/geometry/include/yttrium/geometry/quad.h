// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/geometry/vector.h>

namespace Yt
{
	class Quad
	{
	public:
		Vector2 _a, _b, _c, _d;

		Quad() noexcept = default;
		constexpr Quad(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d) noexcept
			: _a{ a }, _b{ b }, _c{ c }, _d{ d } {}
	};
}
