/// \file
/// \brief

#ifndef _include_yttrium_math_iostream_h_
#define _include_yttrium_math_iostream_h_

#include <yttrium/math/point.h>
#include <yttrium/math/rect.h>

#include <iostream>

namespace Yttrium
{
	inline std::ostream& operator<<(std::ostream& stream, const Point& point)
	{
		return stream << "Point(" << point.x() << ", " << point.y() << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const Rect& rect)
	{
		return stream << "Rect({" << rect.left() << ", " << rect.top() << "}, Size(" << rect.width() << ", " << rect.height() << "))";
	}
}

#endif
