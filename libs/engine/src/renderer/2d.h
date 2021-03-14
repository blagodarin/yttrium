// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/color.h>
#include <yttrium/math/vector.h>

namespace Yt
{
	struct Vertex2D
	{
		Vector2 _position;
		Vector2 _texture;
		Bgra32 _color;
	};
}
