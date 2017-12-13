#include <yttrium/audio/format.h>
#include <yttrium/audio/manager.h>
#include <yttrium/audio/player.h>
#include <yttrium/audio/utils.h>
#include <yttrium/gui/gui.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/args.h>
#include <yttrium/script/context.h>
#include <yttrium/script/value.h>
#include <yttrium/storage/storage.h>
#include <yttrium/storage/writer.h>
#include <yttrium/window.h>

#include "../../utils.h"
#include "graphics.h"
#include "statistics.h"

using namespace Yttrium;

namespace
{
	void make_buttons_texture(Storage& storage, const std::string& name)
	{
		constexpr size_t button_size = 16;
		constexpr size_t button_styles = 4;
		storage.attach_buffer(name, ::make_bgra_tga(button_size, button_size * button_styles, [button_size](size_t, size_t y)
		{
			const auto style = y / button_size;
			return Bgra32{0xff, 0x44 * style, 0x44 * style};
		}));
	}

	void make_cursor_texture(Storage& storage, const std::string& name)
	{
		constexpr size_t size = 64;
		storage.attach_buffer(name, ::make_bgra_tga(size, size, [size](size_t x, size_t y)
		{
			if (y > 2 * x || 2 * y < x || (y > 2 * (size - x) && x > 2 * (size - y)))
				return Bgra32{0, 0, 0, 0};
			else
				return Bgra32
				{
					y * 0xff / (size - 1),
					x * 0xff / (size - 1),
					(size * size - x * y) * 0xff / (size * size),
				};
		}));
	}

	void make_sound(Storage& storage, const std::string& name)
	{
		Buffer buffer;
		{
			constexpr size_t frequency = 44100;
			constexpr size_t duration = frequency / 4; // 0.25 s.

			Writer writer{buffer};
			if (write_wav_header(writer, {2, 1, frequency}, duration))
			{
				constexpr auto time_step = 440.0 / frequency;

				for (size_t i = 0; i < duration; ++i)
				{
					const auto base = std::numeric_limits<int16_t>::max() * std::sin(2 * M_PI * time_step * static_cast<double>(i));
					const auto amplitude = static_cast<double>(duration - i) / duration;
					writer.write(static_cast<int16_t>(base * amplitude));
				}
			}
		}
		storage.attach_buffer(name, std::move(buffer));
	}

	class FieldCanvas : public Canvas
	{
	public:
		FieldCanvas(const Tetrium::Game& logic, const TetriumGraphics& graphics) : _logic{logic}, _graphics{graphics} {}

		void on_draw(RenderPass& pass, const RectF& rect) override
		{
			_graphics.draw_field(pass, rect, _logic.field(), _logic.current_figure());
		}

	private:
		const Tetrium::Game& _logic;
		const TetriumGraphics& _graphics;
	};

	class NextFigureCanvas : public Canvas
	{
	public:
		NextFigureCanvas(const Tetrium::Game& logic, const TetriumGraphics& graphics) : _logic{logic}, _graphics{graphics} {}

		void on_draw(RenderPass& pass, const RectF& rect) override
		{
			_graphics.draw_next_figure(pass, rect, _logic.next_figure());
		}

	private:
		const Tetrium::Game& _logic;
		const TetriumGraphics& _graphics;
	};
}

int main(int, char**)
{
	Window window{"Tetrium"};

	ScriptContext script;

	Tetrium::Game logic;
	script.define("game_pause", [&logic](const ScriptCall&){ logic.pause(); });
	script.define("game_start", [&logic](const ScriptCall& call){ logic.start(call._context.get_int("start_level", 1)); });
	script.define("game_stop", [&logic](const ScriptCall&){ logic.pause(); });
	script.define("game_resume", [&logic](const ScriptCall&){ logic.resume(); });
	script.define("move_down", 1, [&logic](const ScriptCall& call){ logic.set_acceleration(call._args[0]->to_int()); });
	script.define("move_left", 1, [&logic](const ScriptCall& call){ logic.set_left_movement(call._args[0]->to_int()); });
	script.define("move_right", 1, [&logic](const ScriptCall& call){ logic.set_right_movement(call._args[0]->to_int()); });
	script.define("screenshot", [&window](const ScriptCall&){ window.take_screenshot(); });
	script.define("turn_left", [&logic](const ScriptCall&){ logic.turn_left(); });
	script.define("turn_right", [&logic](const ScriptCall&){ logic.turn_right(); });

	TetriumStatistics statistics{script};
	script.define("set_score", 2, [&logic, &statistics](const ScriptCall& call){ statistics.update(call._args[1]->to_int(), call._args[0]->string()); });

	Storage storage{Storage::UseFileSystem::Never};
	storage.attach_package("tetrium.ypq");
	::make_buttons_texture(storage, "examples/tetrium/data/buttons.tga");
	::make_cursor_texture(storage, "examples/tetrium/data/cursor.tga");
	::make_sound(storage, "data/sound.wav");

	AudioManager audio;
	ResourceLoader resource_loader{storage, &window.render_manager(), &audio};
	Gui gui{resource_loader, script, "examples/tetrium/data/gui.ion"};
	gui.on_quit([&window]{ window.close(); });

	window.on_key_event([&gui](const KeyEvent& event){ gui.process_key_event(event); });
	window.on_render([&gui](RenderPass& pass, const Vector2& cursor){ gui.draw(pass, cursor); });
	window.on_screenshot([](Image&& image){ image.save(::make_screenshot_path()); });
	window.on_update([&script, &gui, &logic](const UpdateEvent& event)
	{
		if (logic.advance(static_cast<int>(event.milliseconds.count())))
		{
			script.set("score", logic.score());
			script.set("lines", logic.lines());
			script.set("level", logic.level());
			if (logic.has_finished())
				gui.notify("game_over");
		}
	});

	TetriumGraphics graphics{window.render_manager()};

	FieldCanvas field_canvas{logic, graphics};
	gui.bind_canvas("field", field_canvas);

	NextFigureCanvas next_figure_canvas{logic, graphics};
	gui.bind_canvas("next", next_figure_canvas);

	AudioPlayer audio_player{audio};
	gui.on_music([&audio_player](const std::shared_ptr<MusicReader>& music){ audio_player.set_music(music); });

	gui.start();
	window.show();
	window.run();
}
