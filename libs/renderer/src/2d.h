// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/geometry/vector.h>

#include <seir_graphics/color.hpp>

namespace Yt
{
	struct Vertex2D
	{
		Vector2 _position;
		Vector2 _texture;
		seir::Rgba32 _color;
	};
}
