//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <iosfwd>

namespace Yttrium
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
