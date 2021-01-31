//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include "iostream.h"

#include <yttrium/math/matrix.h>
#include <yttrium/math/point.h>
#include <yttrium/math/rect.h>
#include <yttrium/memory/buffer.h>

#include <sstream>

#include <doctest.h>

namespace Yt
{
	std::ostream& operator<<(std::ostream& stream, const Buffer& buffer)
	{
		return stream << "Buffer(" << buffer.size() << ")";
	}

	std::ostream& operator<<(std::ostream& stream, const Matrix4& m)
	{
		return stream << "Matrix4{{"
					  << m.x.x << "," << m.y.x << "," << m.z.x << "," << m.t.x << "},{"
					  << m.x.y << "," << m.y.y << "," << m.z.y << "," << m.t.y << "},{"
					  << m.x.z << "," << m.y.z << "," << m.z.z << "," << m.t.z << "},{"
					  << m.x.w << "," << m.y.w << "," << m.z.w << "," << m.t.w << "}}";
	}

	std::ostream& operator<<(std::ostream& stream, const Point& point)
	{
		return stream << "Point{" << point._x << "," << point._y << "}";
	}

	std::ostream& operator<<(std::ostream& stream, const Rect& rect)
	{
		return stream << "Rect{{" << rect.left() << "," << rect.top() << "},Size{" << rect.width() << "," << rect.height() << "}}";
	}

	std::ostream& operator<<(std::ostream& stream, const Vector2& v)
	{
		return stream << "Vector2{" << v.x << "," << v.y << "}";
	}

	std::ostream& operator<<(std::ostream& stream, const Vector3& v)
	{
		return stream << "Vector3{" << v.x << "," << v.y << "," << v.z << "}";
	}

	std::ostream& operator<<(std::ostream& stream, const Vector4& v)
	{
		return stream << "Vector4{" << v.x << "," << v.y << "," << v.z << "," << v.w << "}";
	}
}

TEST_CASE("iostream.buffer")
{
	{
		std::ostringstream stream;
		stream << Yt::Buffer{};
		CHECK(stream.str() == "Buffer(0)");
	}
	{
		std::ostringstream stream;
		stream << Yt::Buffer{ 1024 };
		CHECK(stream.str() == "Buffer(1024)");
	}
}

TEST_CASE("iostream.matrix4")
{
	std::ostringstream stream;
	stream << Yt::Matrix4::identity();
	CHECK(stream.str() == "Matrix4{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}");
}

TEST_CASE("iostream.point")
{
	std::ostringstream stream;
	stream << Yt::Point{ 1, 2 };
	CHECK(stream.str() == "Point{1,2}");
}

TEST_CASE("iostream.rect")
{
	std::ostringstream stream;
	stream << Yt::Rect{ { 1, 2 }, Yt::Size{ 3, 4 } };
	CHECK(stream.str() == "Rect{{1,2},Size{3,4}}");
}

TEST_CASE("iostream.vector2")
{
	std::ostringstream stream;
	stream << Yt::Vector2{ 1, 2 };
	CHECK(stream.str() == "Vector2{1,2}");
}

TEST_CASE("iostream.vector3")
{
	std::ostringstream stream;
	stream << Yt::Vector3{ 1, 2, 3 };
	CHECK(stream.str() == "Vector3{1,2,3}");
}

TEST_CASE("iostream.vector4")
{
	std::ostringstream stream;
	stream << Yt::Vector4{ 1, 2, 3, 4 };
	CHECK(stream.str() == "Vector4{1,2,3,4}");
}
