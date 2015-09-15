#ifndef __GUI_TYPES_H
#define __GUI_TYPES_H

#include <cstdint>

namespace Yttrium
{
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
		NotSet,  ///< An unset state.
	};

	enum: WidgetStateType
	{
		WidgetStateCount = WidgetStateType(WidgetState::Count), ///< Number of widget states.
	};
}

#endif
