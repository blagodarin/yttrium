#ifndef _tests_iostream_h_
#define _tests_iostream_h_

#include <yttrium/math/matrix.h>
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

	inline std::ostream& operator<<(std::ostream& stream, const Matrix4& m)
	{
		return stream << "Matrix4{"
			"{" << m(0,0) << "," << m(0,1) << "," << m(0,2) << "," << m(0,3) << "},"
			"{" << m(1,0) << "," << m(1,1) << "," << m(1,2) << "," << m(1,3) << "},"
			"{" << m(2,0) << "," << m(2,1) << "," << m(2,2) << "," << m(2,3) << "},"
			"{" << m(3,0) << "," << m(3,1) << "," << m(3,2) << "," << m(3,3) << "}}";
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
		stream.write(string.text(), static_cast<std::streamsize>(string.size()));
		return stream;
	}
}

#endif
