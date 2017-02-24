#include "iostream.h"

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(test_iostream_buffer)
{
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
	std::ostringstream stream;
	stream << Point{ 1, 2 };
	BOOST_CHECK_EQUAL(stream.str(), "Point(1, 2)");
}

BOOST_AUTO_TEST_CASE(test_iostream_rect)
{
	std::ostringstream stream;
	stream << Rect{ { 1, 2 }, Size{ 3, 4 } };
	BOOST_CHECK_EQUAL(stream.str(), "Rect({1, 2}, Size(3, 4))");
}


BOOST_AUTO_TEST_CASE(test_iostream_static_string)
{
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
