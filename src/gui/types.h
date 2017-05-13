#ifndef _src_gui_types_h_
#define _src_gui_types_h_

namespace Yttrium
{
	enum
	{
		CenterAlignment = 0,

		LeftAlignment = 1 << 0,
		RightAlignment = 1 << 1,
		TopAlignment = 1 << 2,
		BottomAlignment = 1 << 3,

		TopLeftAlignment = TopAlignment | LeftAlignment,
		TopRightAlignment = TopAlignment | RightAlignment,
		BottomLeftAlignment = BottomAlignment | LeftAlignment,
		BottomRightAlignment = BottomAlignment | RightAlignment,

		HorizontalAlignmentMask = LeftAlignment | RightAlignment,
		VerticalAlignmentMask = TopAlignment | BottomAlignment,
	};
}

#endif
