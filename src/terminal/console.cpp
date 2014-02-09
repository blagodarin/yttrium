#include <yttrium/terminal/console.h>

#include <yttrium/renderer/builtin.h>
#include <yttrium/script/context.h>
#include <yttrium/terminal.h>

namespace Yttrium
{

bool Console::process_key(Key key)
{
	if (key == Key::Enter)
	{
		ScriptContext::global().execute(_line_editor.text());
		_line_editor.clear();
		return true;
	}

	if (_line_editor.process_key(key, _terminal.is_shift_pressed()))
	{
		return true;
	}

	char symbol = _terminal.printable(key);

	if (!symbol)
	{
		return false;
	}

	_line_editor.insert(symbol);

	return true;
}

void Console::render_input(RendererBuiltin *renderer, Dim x, Dim y, Dim max_size)
{
	if (_line_editor.selection_size())
	{
		renderer->set_color(1.0, 1.0, 1.0, 0.5);
		renderer->draw_rectangle(x + _line_editor.selection_offset(), y, _line_editor.selection_size(), 1);
	}

	renderer->set_color(1.0, 1.0, 1.0);
	renderer->draw_text(x, y, _line_editor.text(), max_size);

	renderer->set_color(1.0, 0.0, 0.0);
	renderer->draw_cursor(x + _line_editor.cursor(), y);
}

} // namespace Yttrium
