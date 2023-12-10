// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <seir_math/vec.hpp>

namespace Yt
{
	class Quad
	{
	public:
		seir::Vec2 _a, _b, _c, _d;

		Quad() noexcept = default;
		constexpr Quad(const seir::Vec2& a, const seir::Vec2& b, const seir::Vec2& c, const seir::Vec2& d) noexcept
			: _a{ a }, _b{ b }, _c{ c }, _d{ d } {}
	};
}
