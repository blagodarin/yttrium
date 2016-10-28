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
		const unsigned cursor_pos;
		bool           has_cursor = false;
		RectF          cursor_rect;

		const unsigned selection_begin;
		const unsigned selection_end;
		bool           has_selection = false;
		RectF          selection_rect;

		TextCapture(unsigned cursor_pos, unsigned selection_begin, unsigned selection_size)
			: cursor_pos(cursor_pos)
			, selection_begin(selection_begin)
			, selection_end(selection_begin + selection_size)
		{
		}
	};
}

#endif
