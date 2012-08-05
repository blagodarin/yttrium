#include <Yttrium/terminal/console.h>

#include <Yttrium/renderer/builtin.h>
#include <Yttrium/terminal.h>

namespace Yttrium
{

bool Console::process_key(Key key)
{
	switch (key)
	{
	case Key::Left:

		if (_cursor)
		{
			--_cursor;
		}
		break;

	case Key::Right:

		if (_cursor < _input.size())
		{
			++_cursor;
		}
		break;

	case Key::Enter:

		// TODO: Process the input as a script command.
		_input.clear();
		_cursor = 0;
		break;

	case Key::Home:

		_cursor = 0;
		break;

	case Key::End:

		_cursor = _input.size();
		break;

	case Key::Delete:

		_input.remove(_cursor);
		break;

	case Key::Backspace:

		if (_cursor)
		{
			_input.remove(--_cursor);
		}
		break;

	default:

		{
			char symbol = _terminal.printable(key);
			if (!symbol)
			{
				return false;
			}
			_input.insert(symbol, _cursor++);
		}
		break;
	}

	return true;
}

void Console::render_input(RendererBuiltin *renderer, Dim x, Dim y, Dim max_size)
{
	renderer->set_color(1.0, 1.0, 1.0);
	renderer->draw_text(x, y, _input, max_size);
	renderer->set_color(1.0, 0.0, 0.0);
	renderer->draw_cursor(x + _cursor, y);
}

} // namespace Yttrium
