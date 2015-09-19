/// \file
/// \brief 2D size.

#ifndef _include_yttrium_size_h_
#define _include_yttrium_size_h_

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
