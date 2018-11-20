#ifndef _include_yttrium_gui_text_capture_h_
#define _include_yttrium_gui_text_capture_h_

#include <yttrium/math/rect.h>

namespace Yttrium
{
	class TextCapture
	{
	public:
		const std::size_t _cursor_pos;
		bool _has_cursor = false;
		RectF _cursor_rect;

		const std::size_t _selection_begin;
		const std::size_t _selection_end;
		bool _has_selection = false;
		RectF _selection_rect;

		constexpr TextCapture(std::size_t cursor_pos, std::size_t selection_begin, std::size_t selection_size) noexcept
			: _cursor_pos{ cursor_pos }, _selection_begin{ selection_begin }, _selection_end{ _selection_begin + selection_size } {}
	};
}

#endif
