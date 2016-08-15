#ifndef _src_gui_types_h_
#define _src_gui_types_h_

#include <cstdint>

namespace Yttrium
{
	typedef uint_fast8_t WidgetStateType;

	enum class WidgetState: WidgetStateType
	{
		// States that can be set by the manager.

		Normal,
		Active,
		Pressed,
		Checked,

		// Other states.

		Disabled,

		// Special states.

		Count,  // Number of displayed states.
		NotSet, // An unset state.
	};

	enum: WidgetStateType
	{
		WidgetStateCount = WidgetStateType(WidgetState::Count), // Number of widget states.
	};

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
