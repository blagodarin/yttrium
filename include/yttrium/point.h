/// \file
/// \brief 2D point.

#ifndef __Y_POINT_H
#define __Y_POINT_H

namespace Yttrium
{
	/// 2D point.
	struct Point
	{
		int x = 0; ///< X coordinate.
		int y = 0; ///< Y coordinate.

		Point() = default;
		Point(int x, int y) : x(x), y(y) {}
	};
}

#endif
