#include <yttrium/audio/manager.h>
#include <yttrium/audio/player.h>
#include <yttrium/gui/gui.h>
#include <yttrium/image.h>
#include <yttrium/log.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/script/args.h>
#include <yttrium/script/context.h>
#include <yttrium/script/value.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/storage.h>
#include <yttrium/window.h>

#include "../utils.h"
#include "cursor.h"
#include "data.h"
#include "graphics.h"
#include "statistics.h"

using namespace Yttrium;

class Game
{
public:
	Game()
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
		_window.on_update([this](const UpdateEvent& event)
		{
			if (_logic.advance(event.milliseconds.count()))
			{
				_script.set("score", _logic.score());
				_script.set("lines", _logic.lines());
				_script.set("level", _logic.level());
				if (_logic.has_finished())
					_gui.notify("game_over");
			}
		});

		_gui.on_canvas([this](Renderer&, const StaticString& canvas, const RectF& rect){ _graphics.draw(canvas, rect, _logic); });
		_gui.on_custom_cursor([this](Renderer&, const PointF& point){ _cursor.draw(point); });
		_gui.on_music([this](const ResourcePtr<const Music>& music){ _audio_player.set_music(music); });
		_gui.on_quit([this]{ _window.close(); });
	}

	void run()
	{
		_gui.start();
		_window.show();
		_window.run();
	}

private:
	Storage _storage{ Storage::UseFileSystem::Never };
	TetriumData _data{ _storage };
	ScriptContext _script;
	AudioManager _audio;
	AudioPlayer _audio_player{ _audio, AudioPlayer::State::Playing };
	Window _window{ "Tetrium" };
	ResourceLoader _resource_loader{ _storage, &_window.renderer(), &_audio };
	Gui _gui{ _resource_loader, _script, "examples/tetrium/data/gui.ion" };
	Cursor _cursor{ _window.renderer() };
	TetriumGraphics _graphics{ _window.renderer() };
	Tetrium::Game _logic;
	TetriumStatistics _statistics{ _script };
};

int main(int, char**)
{
	Log::set_file("tetrium.log");
	Game game;
	game.run();
	return 0;
}
