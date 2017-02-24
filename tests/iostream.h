#ifndef _tests_iostream_h_
#define _tests_iostream_h_

#include <yttrium/math/point.h>
#include <yttrium/math/rect.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/static_string.h>

#include <iostream>

namespace Yttrium
{
	inline std::ostream& operator<<(std::ostream& stream, const Buffer& buffer)
	{
		return stream << "Buffer(" << buffer.size() << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const Point& point)
	{
		return stream << "Point(" << point.x() << ", " << point.y() << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const Rect& rect)
	{
		return stream << "Rect({" << rect.left() << ", " << rect.top() << "}, Size(" << rect.width() << ", " << rect.height() << "))";
	}

	inline std::ostream& operator<<(std::ostream& stream, const StaticString& string)
	{
		stream.write(string.text(), string.size());
		return stream;
	}
}

#endif
