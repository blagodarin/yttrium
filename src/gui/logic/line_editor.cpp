#include "line_editor.h"

#include <yttrium/key.h>

#include <cassert>
#include <limits>

namespace Yttrium
{
	LineEditor::LineEditor(Allocator& allocator)
		: _text(&allocator)
		, _max_size(std::numeric_limits<decltype(_max_size)>::max())
	{
	}

	void LineEditor::clear()
	{
		_text.clear();
		_cursor = 0;
		_selection_size = 0;
	}

	void LineEditor::insert(const StaticString& text)
	{
		if (_selection_size > 0)
		{
			_text.remove(_selection_offset, _selection_size);
			_cursor = _selection_offset;
			_selection_size = 0;
		}

		if (text.size() <= _max_size - _text.size())
		{
			_text.insert(text, _cursor);
			_cursor += text.size();
		}
	}

	bool LineEditor::process_key(const KeyEvent& event)
	{
		assert(event.pressed);

		const bool shift = event.modifiers & KeyEvent::Shift;

		switch (event.key)
		{
		case Key::Left:

			if (_cursor > 0)
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
				_selection_size = 0;
			break;

		case Key::Right:

			if (_cursor < _text.size())
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
				_selection_size = 0;
			break;

		case Key::Home:

			_selection_size = shift ? _selection_size + _cursor : 0;
			_cursor = 0;
			_selection_offset = 0;
			break;

		case Key::End:

			if (shift && !_selection_size)
				_selection_offset = _cursor;
			_cursor = _text.size();
			_selection_size = shift ? _cursor - _selection_offset : 0;
			break;

		case Key::Delete:

			if (!_selection_size)
			{
				_text.remove(_cursor);
			}
			else
			{
				_text.remove(_selection_offset, _selection_size);
				_cursor = _selection_offset;
				_selection_size = 0;
			}
			break;

		case Key::Backspace:

			if (_selection_size > 0)
			{
				_text.remove(_selection_offset, _selection_size);
				_cursor = _selection_offset;
				_selection_size = 0;
			}
			else if (_cursor > 0)
			{
				_text.remove(--_cursor);
			}
			break;

		default:

			const char event_char = event.to_char();
			if (!event_char)
				return false;
			insert(event_char);
			break;
		}

		return true;
	}

	void LineEditor::set_max_size(unsigned max_size)
	{
		_max_size = max_size;
		_text.truncate(_max_size);
		if (_cursor > _max_size)
			_cursor = _max_size;
		if (_selection_offset > _max_size)
			_selection_offset = _max_size;
		if (_selection_size > _max_size - _selection_offset)
			_selection_size = _max_size - _selection_offset;
	}
}
