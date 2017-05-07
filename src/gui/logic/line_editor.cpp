#include "line_editor.h"

#include <yttrium/key.h>

#include <algorithm>
#include <cassert>

namespace Yttrium
{
	void LineEditor::clear()
	{
		_text.clear();
		_cursor = 0;
		_selection_size = 0;
	}

	void LineEditor::insert(std::string_view text)
	{
		if (_selection_size > 0)
		{
			_text.erase(_selection_offset, _selection_size);
			_cursor = _selection_offset;
			_selection_size = 0;
		}
		if (text.size() <= _max_size - _text.size())
		{
			_text.insert(_cursor, text.data(), text.size());
			_cursor += text.size();
		}
	}

	bool LineEditor::process_key(const KeyEvent& event)
	{
		assert(event.pressed);
		const bool shift = event.modifiers & KeyEvent::Modifier::Shift;
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
				_text.erase(_cursor, 1);
			}
			else
			{
				_text.erase(_selection_offset, _selection_size);
				_cursor = _selection_offset;
				_selection_size = 0;
			}
			break;

		case Key::Backspace:
			if (_selection_size > 0)
			{
				_text.erase(_selection_offset, _selection_size);
				_cursor = _selection_offset;
				_selection_size = 0;
			}
			else if (_cursor > 0)
			{
				_text.erase(--_cursor, 1);
			}
			break;

		default:
			{
				const char event_char = event.to_char();
				if (event_char) // TODO-17: Use init-statement.
				{
					insert(event_char);
					break;
				}
			}
			return false;
		}
		return true;
	}

	void LineEditor::reset(std::string&& text)
	{
		_text = std::move(text);
		_cursor = _text.size();
		_selection_size = 0;
	}

	void LineEditor::set_max_size(size_t max_size)
	{
		_max_size = std::min(max_size, _text.max_size());
		if (_text.size() > _max_size)
			_text.resize(_max_size);
		_cursor = std::min(_cursor, _max_size);
		_selection_offset = std::min(_selection_offset, _max_size);
		_selection_size = std::min(_selection_size, _max_size - _selection_offset);
	}
}
