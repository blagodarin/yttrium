// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/rect.h>

namespace Yt
{
	///
	class TexturedRect
	{
	public:
		RectF geometry; ///<
		RectF texture;  ///<

		TexturedRect() = default;
		TexturedRect(const RectF& geometry_, const RectF& texture_)
			: geometry(geometry_), texture(texture_) {}
	};
}
