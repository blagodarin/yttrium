#include <Yttrium/terminal/console.h>

#include <Yttrium/renderer/builtin.h>
#include <Yttrium/terminal.h>

namespace Yttrium
{

bool Console::process_key(Key key)
{
	bool shift = _terminal.is_shift_pressed();

	switch (key)
	{
	case Key::Left:

		if (_cursor)
		{
			--_cursor;

			if (shift)
			{
				if (_selection_size && _selection_offset <= _cursor)
				{
					--_selection_size;
				}
				else
				{
					++_selection_size;
					_selection_offset = _cursor;
				}
			}
		}
		if (!shift)
		{
			_selection_size = 0;
		}
		break;

	case Key::Right:

		if (_cursor < _input.size())
		{
			if (shift)
			{
				if (_selection_size && _selection_offset == _cursor)
				{
					--_selection_size;
					++_selection_offset;
				}
				else
				{
					_selection_offset = _cursor - _selection_size;
					++_selection_size;
				}
			}

			++_cursor;
		}
		if (!shift)
		{
			_selection_size = 0;
		}
		break;

	case Key::Enter:

		// TODO: Process the input as a script command.
		_input.clear();
		_cursor = 0;
		_selection_size = 0;
		break;

	case Key::Home:

		_selection_size = shift ? _selection_size + _cursor : 0;
		_cursor = 0;
		_selection_offset = 0;
		break;

	case Key::End:

		if (shift && !_selection_size)
		{
			_selection_offset = _cursor;
		}
		_cursor = _input.size();
		_selection_size = shift ? _cursor - _selection_offset : 0;
		break;

	case Key::Delete:

		if (!_selection_size)
		{
			_input.remove(_cursor);
		}
		else
		{
			_input.remove(_selection_offset, _selection_size);
			_cursor = _selection_offset;
			_selection_size = 0;
		}
		break;

	case Key::Backspace:

		if (_selection_size)
		{
			_input.remove(_selection_offset, _selection_size);
			_cursor = _selection_offset;
			_selection_size = 0;
		}
		else if (_cursor)
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

			if (_selection_size)
			{
				_input.remove(_selection_offset, _selection_size);
				_cursor = _selection_offset;
				_selection_size = 0;
			}

			_input.insert(symbol, _cursor++);
		}
		break;
	}

	return true;
}

void Console::render_input(RendererBuiltin *renderer, Dim x, Dim y, Dim max_size)
{
	if (_selection_size)
	{
		renderer->set_color(1.0, 1.0, 1.0, 0.5);
		renderer->draw_image(x + _selection_offset, y, _selection_size, 1);
	}

	renderer->set_color(1.0, 1.0, 1.0);
	renderer->draw_text(x, y, _input, max_size);

	renderer->set_color(1.0, 0.0, 0.0);
	renderer->draw_cursor(x + _cursor, y);
}

} // namespace Yttrium
