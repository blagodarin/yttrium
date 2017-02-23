#include <yttrium/audio/manager.h>
#include <yttrium/audio/player.h>
#include <yttrium/gui/gui.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/script/args.h>
#include <yttrium/script/context.h>
#include <yttrium/script/value.h>
#include <yttrium/storage/storage.h>
#include <yttrium/window.h>

#include "../utils.h"
#include "graphics.h"
#include "statistics.h"

using namespace Yttrium;

namespace
{
	void make_buttons_texture(Storage& storage, const std::string& name)
	{
		constexpr size_t button_size = 16;
		constexpr size_t button_styles = 4;
		storage.attach_buffer(name, ::make_bgra_tga(button_size, button_size * button_styles, [](size_t, size_t y)
		{
			const auto style = y / button_size;
			return std::make_tuple(0xff, 0x44 * style, 0x44 * style, 0xff);
		}));
	}

	void make_cursor_texture(Storage& storage, const std::string& name)
	{
		constexpr size_t size = 64;
		storage.attach_buffer(name, ::make_bgra_tga(size, size, [](size_t x, size_t y) -> std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
		{
			if (y > 2 * x || 2 * y < x || (y > 2 * (size - x) && x > 2 * (size - y)))
				return std::make_tuple(0, 0, 0, 0);
			else
				return std::make_tuple(
					y * 0xff / (size - 1),
					x * 0xff / (size - 1),
					(size * size - x * y) * 0xff / (size * size),
					0xff);
		}));
	}
}

int main(int, char**)
{
	Window window{ "Tetrium" };

	ScriptContext script;

	Tetrium::Game logic;
	script.define("game_pause", [&logic](const ScriptCall&){ logic.pause(); });
	script.define("game_start", [&logic](const ScriptCall& call){ logic.start(call.context.get_int("start_level", 1)); });
	script.define("game_stop", [&logic](const ScriptCall&){ logic.pause(); });
	script.define("game_resume", [&logic](const ScriptCall&){ logic.resume(); });
	script.define("move_down", 1, [&logic](const ScriptCall& call){ logic.set_acceleration(call.args[0]->to_int()); });
	script.define("move_left", 1, [&logic](const ScriptCall& call){ logic.set_left_movement(call.args[0]->to_int()); });
	script.define("move_right", 1, [&logic](const ScriptCall& call){ logic.set_right_movement(call.args[0]->to_int()); });
	script.define("screenshot", [&window](const ScriptCall&){ window.take_screenshot(); });
	script.define("turn_left", [&logic](const ScriptCall&){ logic.turn_left(); });
	script.define("turn_right", [&logic](const ScriptCall&){ logic.turn_right(); });

	TetriumStatistics statistics{ script };
	script.define("set_score", 2, [&logic, &statistics](const ScriptCall& call){ statistics.update(call.args[1]->to_int(), call.args[0]->string()); });

	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_package("tetrium.ypq");
	::make_buttons_texture(storage, "examples/tetrium/data/buttons.tga");
	::make_cursor_texture(storage, "examples/tetrium/data/cursor.tga");

	AudioManager audio;
	ResourceLoader resource_loader{ storage, &window.renderer(), &audio };
	Gui gui{ resource_loader, script, "examples/tetrium/data/gui.ion" };

	window.on_key_event([&gui](const KeyEvent& event){ gui.process_key_event(event); });
	window.on_render([&gui](Renderer& renderer, const PointF& cursor){ gui.render(renderer, cursor); });
	window.on_screenshot([](Image&& image){ image.save(::make_screenshot_path().c_str()); });
	window.on_update([&script, &gui, &logic](const UpdateEvent& event)
	{
		if (logic.advance(event.milliseconds.count()))
		{
			script.set("score", logic.score());
			script.set("lines", logic.lines());
			script.set("level", logic.level());
			if (logic.has_finished())
				gui.notify("game_over");
		}
	});

	TetriumGraphics graphics{ window.renderer() };
	AudioPlayer audio_player{ audio, AudioPlayer::State::Playing };

	gui.on_canvas([&logic, &graphics](Renderer&, const std::string& canvas, const RectF& rect){ graphics.draw(canvas, rect, logic); });
	gui.on_music([&audio_player](const std::shared_ptr<const Music>& music){ audio_player.set_music(music); });
	gui.on_quit([&window]{ window.close(); });

	gui.start();
	window.show();
	window.run();
}
