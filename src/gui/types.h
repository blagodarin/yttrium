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
}

#endif
