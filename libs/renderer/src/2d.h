// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <seir_graphics/color.hpp>
#include <seir_math/vec.hpp>

namespace Yt
{
	struct Vertex2D
	{
		seir::Vec2 _position;
		seir::Vec2 _texture;
		seir::Rgba32 _color;
	};
}
