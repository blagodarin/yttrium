/// \file
/// \brief

#ifndef _include_yttrium_gui_text_capture_h_
#define _include_yttrium_gui_text_capture_h_

#include <yttrium/math/rect.h>

namespace Yttrium
{
	///
	class TextCapture
	{
	public:
		const std::size_t cursor_pos;
		bool              has_cursor = false;
		RectF             cursor_rect;

		const std::size_t selection_begin;
		const std::size_t selection_end;
		bool              has_selection = false;
		RectF             selection_rect;

		TextCapture(std::size_t cursor_pos_, std::size_t selection_begin_, std::size_t selection_size_)
			: cursor_pos(cursor_pos_)
			, selection_begin(selection_begin_)
			, selection_end(selection_begin + selection_size_)
		{
		}
	};
}

#endif
