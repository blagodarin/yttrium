#include <yttrium/audio/manager.h>
#include <yttrium/audio/player.h>
#include <yttrium/gui/gui.h>
#include <yttrium/image.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/script/args.h>
#include <yttrium/script/context.h>
#include <yttrium/script/value.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/storage.h>
#include <yttrium/window.h>

#include "../utils.h"
#include "cursor.h"
#include "graphics.h"
#include "statistics.h"

using namespace Yttrium;

namespace
{
	void make_buttons_texture(Storage& storage, const StaticString& name)
	{
		constexpr auto button_size = 16u;
		constexpr auto button_styles = 4u;
		Image image({ button_size, button_size * button_styles, PixelFormat::Bgra, 32 });
		for (unsigned i = 0; i < button_styles; ++i)
		{
			for (unsigned y = 0; y < button_size; ++y)
			{
				for (unsigned x = 0; x < button_size; ++x)
				{
					const auto pixel = static_cast<uint8_t*>(image.data()) + (i * button_size + y) * image.format().row_size() + x * 4;
					pixel[0] = 0xff;
					pixel[1] = 0x44 * i;
					pixel[2] = 0x44 * i;
					pixel[3] = 0xff;
				}
			}
		}
		storage.attach_buffer(name, image.to_buffer(ImageType::Tga));
	}
}

int main(int, char**)
{
	Window window{ "Tetrium" };

	ScriptContext script;

	Tetrium::Game logic;
	TetriumStatistics statistics{ script };

	script.define("game_pause", [&logic](const ScriptCall&){ logic.pause(); });
	script.define("game_start", [&logic](const ScriptCall& call){ logic.start(call.context.get_int("start_level", 1)); });
	script.define("game_stop", [&logic](const ScriptCall&){ logic.pause(); });
	script.define("game_resume", [&logic](const ScriptCall&){ logic.resume(); });
	script.define("move_down", 1, [&logic](const ScriptCall& call){ logic.set_acceleration(call.args.get_int(0, 0)); });
	script.define("move_left", 1, [&logic](const ScriptCall& call){ logic.set_left_movement(call.args.get_int(0, 0)); });
	script.define("move_right", 1, [&logic](const ScriptCall& call){ logic.set_right_movement(call.args.get_int(0, 0)); });
	script.define("save_score", [&logic, &statistics](const ScriptCall&){ statistics.update(logic.score(), "John Placeholder"); }); // TODO: Get the entered name.
	script.define("set", 2, [](const ScriptCall& call) // TODO: Make built-in, maybe as '='.
	{
		const ScriptValue* value = call.args.value(0);
		if (value->type() == ScriptValue::Type::Name)
			call.context.set(value->to_string(), call.args.string(1));
	});
	script.define("screenshot", [&window](const ScriptCall&){ window.take_screenshot(); });
	script.define("turn_left", [&logic](const ScriptCall&){ logic.turn_left(); });
	script.define("turn_right", [&logic](const ScriptCall&){ logic.turn_right(); });

	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_package("tetrium.ypq");
	::make_buttons_texture(storage, "examples/tetrium/data/buttons.tga");

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
	TetriumCursor cursor{ window.renderer() };
	AudioPlayer audio_player{ audio, AudioPlayer::State::Playing };

	gui.on_canvas([&logic, &graphics](Renderer&, const StaticString& canvas, const RectF& rect){ graphics.draw(canvas, rect, logic); });
	gui.on_custom_cursor([&cursor](Renderer&, const PointF& point){ cursor.draw(point); });
	gui.on_music([&audio_player](const ResourcePtr<const Music>& music){ audio_player.set_music(music); });
	gui.on_quit([&window]{ window.close(); });

	gui.start();
	window.show();
	window.run();
}
