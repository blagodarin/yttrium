#include "iostream.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(iostream_buffer)
{
	using Yttrium::Buffer;
	{
		std::ostringstream stream;
		stream << Buffer{};
		BOOST_CHECK_EQUAL(stream.str(), "Buffer(0)");
	}
	{
		std::ostringstream stream;
		stream << Buffer{1024};
		BOOST_CHECK_EQUAL(stream.str(), "Buffer(1024)");
	}
}

BOOST_AUTO_TEST_CASE(iostream_matrix)
{
	std::ostringstream stream;
	stream << Yttrium::Matrix4::identity();
	BOOST_CHECK_EQUAL(stream.str(), "Matrix4{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}");
}

BOOST_AUTO_TEST_CASE(iostream_point)
{
	std::ostringstream stream;
	stream << Yttrium::Point{1, 2};
	BOOST_CHECK_EQUAL(stream.str(), "Point{1,2}");
}

BOOST_AUTO_TEST_CASE(iostream_rect)
{
	std::ostringstream stream;
	stream << Yttrium::Rect{{1, 2}, Yttrium::Size{3, 4}};
	BOOST_CHECK_EQUAL(stream.str(), "Rect{{1,2},Size{3,4}}");
}

BOOST_AUTO_TEST_CASE(iostream_string)
{
	std::ostringstream stream;
	stream << Yttrium::String{"test"};
	BOOST_CHECK_EQUAL(stream.str(), "test");
}
