/// \file
/// \brief 2D point.

#ifndef _include_yttrium_point_h_
#define _include_yttrium_point_h_

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
