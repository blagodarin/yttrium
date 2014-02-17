#include "commands.h"

#include <yttrium/audio/sound.h>
#include <yttrium/renderer.h>
#include <yttrium/script/context.h>
#include <yttrium/time.h>

#include "game.h"

using namespace std::placeholders;

Commands::Commands(Game *game)
	: _game(game)
{
	ScriptContext::global().define("bind", std::bind(&Commands::bind, this, _1, _2, _3), 2);
	ScriptContext::global().define("exit", std::bind(&Commands::exit, this, _1, _2, _3));
	ScriptContext::global().define("game_pause", std::bind(&Commands::game_pause, this, _1, _2, _3));
	ScriptContext::global().define("game_start", std::bind(&Commands::game_start, this, _1, _2, _3));
	ScriptContext::global().define("game_stop", std::bind(&Commands::game_stop, this, _1, _2, _3));
	ScriptContext::global().define("game_resume", std::bind(&Commands::game_resume, this, _1, _2, _3));
	ScriptContext::global().define("move_down", std::bind(&Commands::move_down, this, _1, _2, _3));
	ScriptContext::global().define("move_left", std::bind(&Commands::move_left, this, _1, _2, _3));
	ScriptContext::global().define("move_right", std::bind(&Commands::move_right, this, _1, _2, _3));
	ScriptContext::global().define("play_music", std::bind(&Commands::play_music, this, _1, _2, _3));
	ScriptContext::global().define("pop_scene", std::bind(&Commands::pop_scene, this, _1, _2, _3), 0, 1);
	ScriptContext::global().define("push_scene", std::bind(&Commands::push_scene, this, _1, _2, _3), 1);
	ScriptContext::global().define("set", std::bind(&Commands::set, this, _1, _2, _3), 2);
	ScriptContext::global().define("seta", std::bind(&Commands::seta, this, _1, _2, _3), 2);
	ScriptContext::global().define("snap", std::bind(&Commands::snap, this, _1, _2, _3));
	ScriptContext::global().define("stop_music", std::bind(&Commands::stop_music, this, _1, _2, _3));
	ScriptContext::global().define("tgcon", std::bind(&Commands::tgcon, this, _1, _2, _3));
	ScriptContext::global().define("turn_left", std::bind(&Commands::turn_left, this, _1, _2, _3));
	ScriptContext::global().define("turn_right", std::bind(&Commands::turn_right, this, _1, _2, _3));
	ScriptContext::global().define("unbindall", std::bind(&Commands::unbindall, this, _1, _2, _3));
	ScriptContext::global().define("unset", std::bind(&Commands::unset, this, _1, _2, _3), 1);
}

void Commands::bind(const StaticString &, String *, const ScriptArgs &args) noexcept
{
	_game->_bindings.bind(args.string(0), args.string(1));
}

void Commands::exit(const StaticString &, String *, const ScriptArgs &) noexcept
{
	_game->_terminal.close();
}

void Commands::game_pause(const StaticString &, String *, const ScriptArgs &) noexcept
{
	_game->_game_timer.stop();
}

void Commands::game_start(const StaticString &, String *, const ScriptArgs &) noexcept
{
	ScriptValue *start_level_value = ScriptContext::global().find("start_level");
	int start_level = start_level_value ? start_level_value->integer() : 1;
	_game->_game.start(start_level);
	_game->_game_timer.start();
}

void Commands::game_stop(const StaticString &, String *, const ScriptArgs &) noexcept
{
	_game->_game_timer.stop();
	_game->_game_timer.reset();
}

void Commands::game_resume(const StaticString &, String *, const ScriptArgs &) noexcept
{
	_game->_game_timer.start();
}

void Commands::move_down(const StaticString &name, String *, const ScriptArgs &) noexcept
{
	if (_game->_game_timer.is_started())
		_game->_game.set_acceleration(name[0] == '+');
}

void Commands::move_left(const StaticString &name, String *, const ScriptArgs &) noexcept
{
	if (_game->_game_timer.is_started())
		_game->_game.set_left_movement(name[0] == '+');
}

void Commands::move_right(const StaticString &name, String *, const ScriptArgs &) noexcept
{
	if (_game->_game_timer.is_started())
		_game->_game.set_right_movement(name[0] == '+');
}

void Commands::play_music(const StaticString &, String *, const ScriptArgs &) noexcept
{
	if (!_game->_audio.player().is_playing())
		_game->_audio.player().play();
	else
		_game->_audio.player().pause();
}

void Commands::pop_scene(const StaticString &, String *, const ScriptArgs &args) noexcept
{
	Integer scenes_to_pop = !args.size() ? 1 : args.value(0)->integer();

	if (scenes_to_pop > 0 && !_game->_gui->pop_scenes(scenes_to_pop))
		_game->_terminal.close();
}

void Commands::push_scene(const StaticString &, String *, const ScriptArgs &args) noexcept
{
	_game->_gui->push_scene(args.string(0));
}

void Commands::set(const StaticString &, String *, const ScriptArgs &args) noexcept
{
	const ScriptValue *value = args.value(0);
	if (value->type() == ScriptValue::Name)
		ScriptContext::global().set(value->string(), args.string(1, ScriptArgs::Resolve));
}

void Commands::seta(const StaticString &, String *, const ScriptArgs &args) noexcept
{
	const ScriptValue *value = args.value(0);
	if (value->type() == ScriptValue::Name)
		ScriptContext::global().set(value->string(), args.string(1, ScriptArgs::Resolve), ScriptValue::Archived);
}

void Commands::snap(const StaticString &, String *, const ScriptArgs &) noexcept
{
	// NOTE: This won't allow us to take more than one screenshot per second,
	// but do we need such a feature?

	DateTime now = DateTime::now();
	_game->_renderer.take_screenshot(String(24, _game->_allocator)
		.append_dec(now.year, 4, true)
		.append('-')
		.append_dec(now.month, 2, true)
		.append('-')
		.append_dec(now.day, 2, true)
		.append('-')
		.append_dec(now.hour, 2, true)
		.append('-')
		.append_dec(now.minute, 2, true)
		.append('-')
		.append_dec(now.second, 2, true)
		.append(".png"));
}

void Commands::stop_music(const StaticString &, String *, const ScriptArgs &) noexcept
{
	_game->_audio.player().stop();
}

void Commands::tgcon(const StaticString &, String *, const ScriptArgs &) noexcept
{
	_game->_terminal.set_console_visible(!_game->_terminal.is_console_visible());
}

void Commands::turn_left(const StaticString &, String *, const ScriptArgs &) noexcept
{
	if (_game->_game_timer.is_started())
		_game->_game.turn_left();
}

void Commands::turn_right(const StaticString &, String *, const ScriptArgs &) noexcept
{
	if (_game->_game_timer.is_started())
		_game->_game.turn_right();
}

void Commands::unbindall(const StaticString &, String *, const ScriptArgs &) noexcept
{
	_game->_bindings.clear();
}

void Commands::unset(const StaticString &, String *, const ScriptArgs &args) noexcept
{
	const ScriptValue *value = args.value(0);
	if (value->type() == ScriptValue::Name)
		ScriptContext::global().unset(value->string());
}
