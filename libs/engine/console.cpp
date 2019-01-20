//
// Copyright 2019 Sergei Blagodarin
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

#include <yttrium/console.h>

#include <yttrium/key.h>
#include <yttrium/script/code.h>
#include <yttrium/script/context.h>
#include "gui/logic/line_editor.h"
#include "renderer/debug_renderer.h"

namespace Yttrium
{
	class ConsolePrivate
	{
	public:
		explicit ConsolePrivate(ScriptContext& script_context)
			: _script_context{ script_context } {}

	public:
		ScriptContext& _script_context;
		LineEditor _line_editor;
		bool _visible = false;
	};

	Console::Console(ScriptContext& script_context)
		: _private{ std::make_unique<ConsolePrivate>(script_context) }
	{
	}

	Console::~Console() = default;

	void Console::draw(RenderPass& pass) const
	{
		if (!_private->_visible)
			return;

		DebugRenderer renderer{ pass };

		const auto max_width = renderer.max_width();

		renderer.set_color(0, 0, 0, 0.5);
		renderer.draw_rectangle(0, 0, max_width + 1, 1);

		if (_private->_line_editor.selection_size())
		{
			renderer.set_color(1.0, 1.0, 1.0, 0.5);
			renderer.draw_rectangle(_private->_line_editor.selection_offset(), 0, _private->_line_editor.selection_size(), 1);
		}

		renderer.set_color(1.0, 1.0, 1.0);
		renderer.draw_text(0, 0, _private->_line_editor.text(), max_width);

		renderer.set_color(1.0, 0.0, 0.0);
		renderer.draw_cursor(_private->_line_editor.cursor(), 0);
	}

	bool Console::is_visible() const
	{
		return _private->_visible;
	}

	bool Console::process_key_event(const KeyEvent& event)
	{
		if (_private->_visible && event._pressed)
		{
			switch (event._key)
			{
			case Key::Escape:
				_private->_visible = false;
				return true;

			case Key::Enter:
				ScriptCode{ std::string{ _private->_line_editor.text() } }.execute(_private->_script_context);
				_private->_line_editor.clear();
				return true;

			default:
				return _private->_line_editor.process_key(event);
			}
		}
		return false;
	}

	void Console::set_visible(bool visible)
	{
		_private->_visible = visible;
	}
}
