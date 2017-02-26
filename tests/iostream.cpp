#include "iostream.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_iostream_buffer)
{
	using Yttrium::Buffer;

	{
		std::ostringstream stream;
		stream << Buffer{};
		BOOST_CHECK_EQUAL(stream.str(), "Buffer(0)");
	}
	{
		std::ostringstream stream;
		stream << Buffer{ 1024 };
		BOOST_CHECK_EQUAL(stream.str(), "Buffer(1024)");
	}
}

BOOST_AUTO_TEST_CASE(test_iostream_point)
{
	using Yttrium::Point;

	std::ostringstream stream;
	stream << Point{ 1, 2 };
	BOOST_CHECK_EQUAL(stream.str(), "Point(1, 2)");
}

BOOST_AUTO_TEST_CASE(test_iostream_rect)
{
	using Yttrium::Rect;
	using Yttrium::Size;

	std::ostringstream stream;
	stream << Rect{ { 1, 2 }, Size{ 3, 4 } };
	BOOST_CHECK_EQUAL(stream.str(), "Rect({1, 2}, Size(3, 4))");
}


BOOST_AUTO_TEST_CASE(test_iostream_static_string)
{
	using namespace Yttrium::Literals;

	{
		std::ostringstream stream;
		stream << ""_s;
		BOOST_CHECK_EQUAL(stream.str(), "");
	}
	{
		std::ostringstream stream;
		stream << "a"_s;
		BOOST_CHECK_EQUAL(stream.str(), "a");
	}
	{
		std::ostringstream stream;
		stream << "abcdefghijklmnopqrstuvwxyz"_s;
		BOOST_CHECK_EQUAL(stream.str(), "abcdefghijklmnopqrstuvwxyz");
	}
}
