#include <yttrium/console.h>

#include <yttrium/key.h>
#include <yttrium/script/code.h>
#include <yttrium/script/context.h>
#include "gui/logic/line_editor.h"
#include "renderer/debug_renderer.h"
#include "renderer/renderer.h"

namespace Yttrium
{
	class ConsolePrivate
	{
	public:
		explicit ConsolePrivate(ScriptContext& script_context) : _script_context{script_context} {}

	public:
		ScriptContext& _script_context;
		LineEditor _line_editor;
		bool _visible = false;
	};

	Console::Console(ScriptContext& script_context)
		: _private{std::make_unique<ConsolePrivate>(script_context)}
	{
	}

	Console::~Console() = default;

	void Console::draw(RenderContext& context) const
	{
		if (!_private->_visible)
			return;

		DebugRenderer debug{static_cast<RendererImpl&>(context)};

		const auto max_width = debug.max_width();

		debug.set_color(0, 0, 0, 0.5);
		debug.draw_rectangle(0, 0, max_width + 1, 1);

		if (_private->_line_editor.selection_size())
		{
			debug.set_color(1.0, 1.0, 1.0, 0.5);
			debug.draw_rectangle(_private->_line_editor.selection_offset(), 0, _private->_line_editor.selection_size(), 1);
		}

		debug.set_color(1.0, 1.0, 1.0);
		debug.draw_text(0, 0, _private->_line_editor.text(), max_width);

		debug.set_color(1.0, 0.0, 0.0);
		debug.draw_cursor(_private->_line_editor.cursor(), 0);
	}

	bool Console::is_visible() const
	{
		return _private->_visible;
	}

	bool Console::process_key_event(const KeyEvent& event)
	{
		if (_private->_visible && event.pressed)
		{
			switch (event.key)
			{
			case Key::Escape:
				_private->_visible = false;
				return true;

			case Key::Enter:
				ScriptCode{std::string{_private->_line_editor.text()}}.execute(_private->_script_context);
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
