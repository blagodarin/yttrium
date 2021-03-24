// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iosfwd>

namespace Yt
{
	class Buffer;
	class Matrix4;
	class Point;
	class Rect;
	class Vector2;
	class Vector3;
	class Vector4;

	std::ostream& operator<<(std::ostream&, const Buffer&);
	std::ostream& operator<<(std::ostream&, const Matrix4&);
	std::ostream& operator<<(std::ostream&, const Point&);
	std::ostream& operator<<(std::ostream&, const Rect&);
	std::ostream& operator<<(std::ostream&, const Vector2&);
	std::ostream& operator<<(std::ostream&, const Vector3&);
	std::ostream& operator<<(std::ostream&, const Vector4&);
}
