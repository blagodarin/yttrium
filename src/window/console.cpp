#include "console.h"

#include <yttrium/renderer/builtin.h>
#include <yttrium/script/code.h>
#include <yttrium/window.h>

#include <cassert>

namespace Yttrium
{

bool Console::process_key(const KeyEvent& event)
{
	assert(event.pressed);

	if (event.key == Key::Enter)
	{
		ScriptCode(_line_editor.text()).execute();
		_line_editor.clear();
		return true;
	}

	return _line_editor.process_key(event);
}

void Console::render_input(RendererBuiltin& renderer, int x, int y, int max_size)
{
	if (_line_editor.selection_size())
	{
		renderer.set_color(1.0, 1.0, 1.0, 0.5);
		renderer.draw_rectangle(x + _line_editor.selection_offset(), y, _line_editor.selection_size(), 1);
	}

	renderer.set_color(1.0, 1.0, 1.0);
	renderer.draw_text(x, y, _line_editor.text(), max_size);

	renderer.set_color(1.0, 0.0, 0.0);
	renderer.draw_cursor(x + _line_editor.cursor(), y);
}

} // namespace Yttrium
