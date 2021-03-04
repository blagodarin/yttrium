//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "line_editor.h"

#include <yttrium/key.h>

#include <primal/utf8.hpp>

#include <algorithm>
#include <cassert>

namespace Yt
{
	void LineEditor::clear() noexcept
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
		if (text.size() <= _max_bytes - _text.size())
		{
			_text.insert(_cursor, text.data(), text.size());
			_cursor += text.size();
		}
	}

	bool LineEditor::process_key(const KeyEvent& event)
	{
		assert(event._pressed);
		const bool shift = event._modifiers & KeyEvent::Modifier::Shift;
		switch (event._key)
		{
		case Key::Left:
			if (_cursor > 0)
			{
				const auto amount = left_codepoint_bytes();
				_cursor -= amount;
				if (shift)
				{
					if (_selection_size > 0 && _selection_offset <= _cursor)
					{
						_selection_size -= amount;
					}
					else
					{
						_selection_size += amount;
						_selection_offset = _cursor;
					}
				}
			}
			if (!shift)
				_selection_size = 0;
			return true;

		case Key::Right:
			if (_cursor < _text.size())
			{
				const auto amount = right_codepoint_bytes();
				if (shift)
				{
					if (_selection_size > 0 && _selection_offset == _cursor)
					{
						_selection_size -= amount;
						_selection_offset += amount;
					}
					else
					{
						_selection_offset = _cursor - _selection_size;
						_selection_size += amount;
					}
				}
				_cursor += amount;
			}
			if (!shift)
				_selection_size = 0;
			return true;

		case Key::Home:
			_selection_size = shift ? _selection_size + _cursor : 0;
			_cursor = 0;
			_selection_offset = 0;
			return true;

		case Key::End:
			if (shift && !_selection_size)
				_selection_offset = _cursor;
			_cursor = _text.size();
			_selection_size = shift ? _cursor - _selection_offset : 0;
			return true;

		case Key::Delete:
			if (_selection_size > 0)
			{
				_text.erase(_selection_offset, _selection_size);
				_cursor = _selection_offset;
				_selection_size = 0;
			}
			else if (_cursor < _text.size())
				_text.erase(_cursor, right_codepoint_bytes());
			return true;

		case Key::Backspace:
			if (_selection_size > 0)
			{
				_text.erase(_selection_offset, _selection_size);
				_cursor = _selection_offset;
				_selection_size = 0;
			}
			else if (_cursor > 0)
			{
				const auto amount = left_codepoint_bytes();
				_cursor -= amount;
				_text.erase(_cursor, amount);
			}
			return true;

		default:
			return false;
		}
	}

	void LineEditor::reset(std::string&& text) noexcept
	{
		_text = std::move(text);
		_cursor = _text.size();
		_selection_size = 0;
	}

	void LineEditor::set_max_bytes(std::size_t count) noexcept
	{
		_max_bytes = std::min(count, _text.max_size());
		if (_text.size() > _max_bytes)
		{
			auto new_text_size = _max_bytes;
			while (new_text_size > 0 && primal::isUtf8Continuation(_text[new_text_size]))
				--new_text_size;
			_text.resize(new_text_size);
		}
		_cursor = std::min(_cursor, _max_bytes);
		_selection_offset = std::min(_selection_offset, _max_bytes);
		_selection_size = std::min(_selection_size, _max_bytes - _selection_offset);
	}

	std::size_t LineEditor::left_codepoint_bytes() const noexcept
	{
		assert(_cursor > 0);
		std::size_t result = 0;
		do
			++result;
		while (_cursor > result && primal::isUtf8Continuation(_text[_cursor - result]));
		return result;
	}

	std::size_t LineEditor::right_codepoint_bytes() const noexcept
	{
		assert(_cursor < _text.size());
		std::size_t result = 0;
		const auto max_result = _text.size() - _cursor;
		do
			++result;
		while (result < max_result && primal::isUtf8Continuation(_text[_cursor + result]));
		return result;
	}
}
