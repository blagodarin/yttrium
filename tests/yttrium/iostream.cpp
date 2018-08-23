#include "iostream.h"

#include <sstream>

#include <catch/catch.hpp>

TEST_CASE("iostream.buffer")
{
	using Yttrium::Buffer;
	{
		std::ostringstream stream;
		stream << Buffer{};
		CHECK(stream.str() == "Buffer(0)");
	}
	{
		std::ostringstream stream;
		stream << Buffer{1024};
		CHECK(stream.str() == "Buffer(1024)");
	}
}

TEST_CASE("iostream.matrix4")
{
	std::ostringstream stream;
	stream << Yttrium::Matrix4::identity();
	CHECK(stream.str() == "Matrix4{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}");
}

TEST_CASE("iostream.point")
{
	std::ostringstream stream;
	stream << Yttrium::Point{1, 2};
	CHECK(stream.str() == "Point{1,2}");
}

TEST_CASE("iostream.rect")
{
	std::ostringstream stream;
	stream << Yttrium::Rect{{1, 2}, Yttrium::Size{3, 4}};
	CHECK(stream.str() == "Rect{{1,2},Size{3,4}}");
}
