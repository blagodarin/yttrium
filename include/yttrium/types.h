/// \file
/// \brief

#ifndef _include_yttrium_types_h_
#define _include_yttrium_types_h_

namespace Yttrium
{
	enum
	{
		CenterAlignment = 0, ///< Center alignment.

		LeftAlignment   = 1 << 0, ///< Left alignment.
		RightAlignment  = 1 << 1, ///< Right alignment.
		TopAlignment    = 1 << 2, ///< Top alignment.
		BottomAlignment = 1 << 3, ///< Bottom alignment.

		TopLeftAlignment     = TopAlignment | LeftAlignment,     ///< Top left alignment.
		TopRightAlignment    = TopAlignment | RightAlignment,    ///< Top right alignment.
		BottomLeftAlignment  = BottomAlignment | LeftAlignment,  ///< Bottom left alignment.
		BottomRightAlignment = BottomAlignment | RightAlignment, ///< Bottom right alignment.

		HorizontalAlignmentMask = LeftAlignment | RightAlignment, ///< Horizontal alignment mask.
		VerticalAlignmentMask   = TopAlignment | BottomAlignment, ///< Vertical alignment mask.
	};
}

#endif
