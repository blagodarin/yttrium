#include "game.h"

#include <yttrium/image.h>
#include <yttrium/script/args.h>
#include <yttrium/script/value.h>
#include <yttrium/string_format.h>
#include <yttrium/time.h>

Game::Game(Storage& storage)
	: _storage(storage)
	, _statistics
		{
			{ 100000, "John Placeholder" },
			{ 50000, "John Placeholder" },
			{ 10000, "John Placeholder" },
			{ 5000, "John Placeholder" },
			{ 1000, "John Placeholder" },
		}
{
	_script.define("game_pause", [this](const ScriptCall&)
	{
		_logic.pause();
	});

	_script.define("game_start", [this](const ScriptCall& call)
	{
		_logic.start(call.context.get_int("start_level", 1));
	});

	_script.define("game_stop", [this](const ScriptCall&)
	{
		_logic.pause();
	});

	_script.define("game_resume", [this](const ScriptCall&)
	{
		_logic.resume();
	});

	_script.define("move_down", [this](const ScriptCall& call)
	{
		_logic.set_acceleration(call.function[0] == '+');
	});

	_script.define("move_left", [this](const ScriptCall& call)
	{
		_logic.set_left_movement(call.function[0] == '+');
	});

	_script.define("move_right", [this](const ScriptCall& call)
	{
		_logic.set_right_movement(call.function[0] == '+');
	});

	_script.define("save_score", [this](const ScriptCall&)
	{
		_statistics.emplace(_logic.score(), "John Placeholder"); // TODO: Get the entered name.
		_statistics.erase(_statistics.begin());
		update_statistics();
	});

	_script.define("set", 2, [this](const ScriptCall& call)
	{
		const ScriptValue* value = call.args.value(0);
		if (value->type() == ScriptValue::Type::Name)
			call.context.set(value->to_string(), call.args.string(1, ScriptArgs::Resolve));
	});

	_script.define("screenshot", [this](const ScriptCall&)
	{
		_window.take_screenshot();
	});

	_script.define("turn_left", [this](const ScriptCall&)
	{
		_logic.turn_left();
	});

	_script.define("turn_right", [this](const ScriptCall&)
	{
		_logic.turn_right();
	});

	_window.on_key_event([this](const KeyEvent& event)
	{
		_gui.process_key_event(event);
	});

	_window.on_render([this](Renderer& renderer, const PointF& cursor)
	{
		_gui.render(renderer, cursor);
	});

	_window.on_screenshot([this](Image&& image)
	{
		image.save(String() << print(DateTime::now(), "%YY-%MM-%DD_%hh-%mm-%ss.png"), ImageType::Auto);
	});

	_window.on_update([this](const UpdateEvent& update)
	{
		if (_logic.advance(update.milliseconds))
		{
			_script.set("score", _logic.score());
			_script.set("lines", _logic.lines());
			_script.set("level", _logic.level());
			if (_logic.has_finished())
				_gui.notify("game_over");
		}
	});

	_gui.on_canvas("field", [this](Renderer&, const RectF& rect)
	{
		_graphics.draw_field(rect, _logic.field(), _logic.current_figure());
	});

	_gui.on_canvas("next", [this](Renderer&, const RectF& rect)
	{
		_graphics.draw_next_figure(rect, _logic.next_figure());
	});

	_gui.on_custom_cursor([this](Renderer&, const PointF& point)
	{
		_cursor.draw(point);
	});

	_gui.on_quit([this]
	{
		_window.close();
	});

	update_statistics();
}

Game::~Game() = default;

void Game::run()
{
	_gui.start();
	_window.show();
	_window.run();
}

void Game::update_statistics()
{
	int index = 0;
	for (auto i = _statistics.rbegin(); i != _statistics.rend(); ++i)
	{
		++index;
		_script.set(String() << "name" << index, i->second.c_str());
		_script.set(String() << "score" << index, i->first);
	}
}
