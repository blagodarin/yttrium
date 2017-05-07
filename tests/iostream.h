#ifndef _tests_iostream_h_
#define _tests_iostream_h_

#include <yttrium/math/matrix.h>
#include <yttrium/math/point.h>
#include <yttrium/math/rect.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/string.h>

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
			"{" << m.x.x << "," << m.y.x << "," << m.z.x << "," << m.t.x << "},"
			"{" << m.x.y << "," << m.y.y << "," << m.z.y << "," << m.t.y << "},"
			"{" << m.x.z << "," << m.y.z << "," << m.z.z << "," << m.t.z << "},"
			"{" << m.x.w << "," << m.y.w << "," << m.z.w << "," << m.t.w << "}}";
	}

	inline std::ostream& operator<<(std::ostream& stream, const Point& point)
	{
		return stream << "Point{" << point._x << "," << point._y << "}";
	}

	inline std::ostream& operator<<(std::ostream& stream, const Rect& rect)
	{
		return stream << "Rect{{" << rect.left() << "," << rect.top() << "},Size{" << rect.width() << "," << rect.height() << "}}";
	}

	inline std::ostream& operator<<(std::ostream& stream, const String& string)
	{
		return stream << std::string_view{string};
	}
}

#endif
