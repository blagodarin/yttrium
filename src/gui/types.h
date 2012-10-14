#ifndef __GUI_TYPES_H
#define __GUI_TYPES_H

#include <Yttrium/types.h>

namespace Yttrium
{

namespace Gui
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

///

typedef uint_fast8_t Alignment;

/// Widget scaling mode.

enum class Scaling
{
	Stretch, ///< Scale the widget into the region.
	Min,     ///< Scale the widget proportionally until it fits the region.
	Max,     ///< Scale the widget proportionally until it covers the region.
	Fit,     ///< Stretch for position, Min for size.
};

///

typedef uint_fast8_t WidgetStateType;

///

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

	Count, ///< Number of displayed states.
	None,  ///< An unset state.
};

enum: WidgetStateType
{
	WidgetStateCount = WidgetStateType(WidgetState::Count), ///< Number of widget states.
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_TYPES_H
