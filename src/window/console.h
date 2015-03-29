#ifndef __WINDOW_CONSOLE_H
#define __WINDOW_CONSOLE_H

#include "../gui/logic/line_editor.h"

namespace Yttrium
{
	class DebugRenderer;

	class Console
	{
	public:

		Console(Allocator* allocator)
			: _line_editor(allocator)
		{
		}

		bool process_key(const KeyEvent& event);
		void draw_input(DebugRenderer& renderer, int x, int y, int max_size);

	private:

		LineEditor _line_editor;
	};
}

#endif // __WINDOW_CONSOLE_H
