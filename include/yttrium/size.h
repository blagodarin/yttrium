/// \file
/// \brief 2D size.

#ifndef __Y_SIZE_H
#define __Y_SIZE_H

namespace Yttrium
{
	/// 2D size.
	struct Size
	{
		int width = 0;  ///<
		int height = 0; ///<

		Size() = default;
		Size(int width, int height) : width(width), height(height) {}
	};
}

#endif
