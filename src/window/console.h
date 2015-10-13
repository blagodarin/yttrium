#ifndef _src_window_console_h_
#define _src_window_console_h_

#include "../gui/logic/line_editor.h"

namespace Yttrium
{
	class DebugRenderer;
	class ScriptContext;

	class Console
	{
	public:

		Console(ScriptContext&, Allocator&);

		bool process_key(const KeyEvent&);
		void draw_input(DebugRenderer&, int x, int y, int max_size);

	private:

		ScriptContext& _script_context;
		LineEditor _line_editor;
	};
}

#endif
