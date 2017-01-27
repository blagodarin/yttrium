#include "game.h"

#include "../utils.h"

#include <yttrium/image.h>
#include <yttrium/script/args.h>
#include <yttrium/script/value.h>
#include <yttrium/string_format.h>

Game::Game(Storage& storage)
	: _storage(storage)
{
	_script.define("game_pause", [this](const ScriptCall&){ _logic.pause(); });
	_script.define("game_start", [this](const ScriptCall& call){ _logic.start(call.context.get_int("start_level", 1)); });
	_script.define("game_stop", [this](const ScriptCall&){ _logic.pause(); });
	_script.define("game_resume", [this](const ScriptCall&){ _logic.resume(); });
	_script.define("move_down", [this](const ScriptCall& call){ _logic.set_acceleration(call.function[0] == '+'); });
	_script.define("move_left", [this](const ScriptCall& call){ _logic.set_left_movement(call.function[0] == '+'); });
	_script.define("move_right", [this](const ScriptCall& call){ _logic.set_right_movement(call.function[0] == '+'); });
	_script.define("save_score", [this](const ScriptCall&){ _statistics.update(_logic.score(), "John Placeholder"); }); // TODO: Get the entered name.
	_script.define("set", 2, [this](const ScriptCall& call) // TODO: Make built-in, maybe as '='.
	{
		const ScriptValue* value = call.args.value(0);
		if (value->type() == ScriptValue::Type::Name)
			call.context.set(value->to_string(), call.args.string(1, ScriptArgs::Resolve));
	});
	_script.define("screenshot", [this](const ScriptCall&){ _window.take_screenshot(); });
	_script.define("turn_left", [this](const ScriptCall&){ _logic.turn_left(); });
	_script.define("turn_right", [this](const ScriptCall&){ _logic.turn_right(); });

	_window.on_key_event([this](const KeyEvent& event){ _gui.process_key_event(event); });
	_window.on_render([this](Renderer& renderer, const PointF& cursor){ _gui.render(renderer, cursor); });
	_window.on_screenshot([this](Image&& image){ image.save(::make_screenshot_path().c_str()); });
	_window.on_update([this](const UpdateEvent& event){ update(std::chrono::milliseconds(event.milliseconds)); });

	_gui.on_canvas([this](Renderer&, const StaticString& canvas, const RectF& rect){ _graphics.draw(canvas, rect, _logic); });
	_gui.on_custom_cursor([this](Renderer&, const PointF& point){ _cursor.draw(point); });
	_gui.on_quit([this]{ _window.close(); });
}

void Game::run()
{
	_gui.start();
	_window.show();
	_window.run();
}

void Game::update(const std::chrono::milliseconds& milliseconds)
{
	if (_logic.advance(milliseconds.count()))
	{
		_script.set("score", _logic.score());
		_script.set("lines", _logic.lines());
		_script.set("level", _logic.level());
		if (_logic.has_finished())
			_gui.notify("game_over");
	}
}
