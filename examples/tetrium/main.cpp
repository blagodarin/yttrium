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

#include <yttrium/application.h>
#include <yttrium/audio/format.h>
#include <yttrium/audio/manager.h>
#include <yttrium/audio/player.h>
#include <yttrium/audio/utils.h>
#include <yttrium/exceptions.h>
#include <yttrium/gui/gui.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/pass.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/args.h>
#include <yttrium/script/context.h>
#include <yttrium/script/value.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/storage.h>
#include <yttrium/storage/writer.h>
#include <yttrium/window.h>

#include "../utils.h"
#include "graphics.h"

using namespace Yttrium;

namespace
{
	template <std::size_t button_size>
	void make_buttons_texture(Storage& storage, const std::string& name)
	{
		constexpr size_t button_styles = 4;
		storage.attach_buffer(name, ::make_bgra_tga(button_size, button_size * button_styles, [](size_t, size_t y) {
			const auto style = y / button_size;
			return Bgra32{ 0xff, 0x44 * style, 0x44 * style };
		}));
	}

	template <std::size_t size>
	void make_cursor_texture(Storage& storage, const std::string& name)
	{
		storage.attach_buffer(name, ::make_bgra_tga(size, size, [](size_t x, size_t y) {
			if (y > 2 * x || 2 * y < x || (y > 2 * (size - x) && x > 2 * (size - y)))
				return Bgra32{ 0, 0, 0, 0 };
			else
				return Bgra32{
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

			Writer writer{ buffer };
			if (write_wav_header(writer, { 2, 1, frequency }, duration))
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

	class LogoCanvas : public Canvas
	{
	public:
		LogoCanvas(Gui& gui)
			: _gui{ gui } {}

		void on_draw(RenderPass& pass, const RectF& rect, std::chrono::milliseconds screen_time) override
		{
			const auto ms = screen_time.count();
			if (ms >= 4000)
				_gui.notify("finished");
			const auto opacity = std::min(1.f, std::abs(static_cast<float>(ms - 500) / 1500.f - 1.f));
			PushTexture texture{ pass, nullptr };
			pass.draw_rect(rect, { 0.f, 0.f, 0.f, opacity });
		}

	private:
		Gui& _gui;
	};

	class FieldCanvas : public Canvas
	{
	public:
		FieldCanvas(const Tetrium::Game& logic, const TetriumGraphics& graphics)
			: _logic{ logic }, _graphics{ graphics } {}

		void on_draw(RenderPass& pass, const RectF& rect, std::chrono::milliseconds) override
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
		NextFigureCanvas(const Tetrium::Game& logic, const TetriumGraphics& graphics)
			: _logic{ logic }, _graphics{ graphics } {}

		void on_draw(RenderPass& pass, const RectF& rect, std::chrono::milliseconds) override
		{
			_graphics.draw_next_figure(pass, rect, _logic.next_figure());
		}

	private:
		const Tetrium::Game& _logic;
		const TetriumGraphics& _graphics;
	};

	class ScoreTable
	{
	public:
		explicit ScoreTable(ScriptContext& script)
			: _script{ script } {}

		void add_score(const std::string& name, int score)
		{
			const auto i = std::find_if(_scores.cbegin(), _scores.cend(), [score](const auto& table_score) { return table_score.second < score; });
			if (i != _scores.cend())
			{
				_scores.emplace(i, name, score);
				_scores.resize(_scores.size() - 1);
				update_variables();
			}
		}

		void set_score_count(int count)
		{
			_scores.resize(static_cast<std::size_t>(count));
			update_variables();
		}

	private:
		void update_variables() const
		{
			for (const auto& score : _scores)
			{
				const auto index = &score - _scores.data() + 1;
				_script.set("name" + std::to_string(index), score.first);
				_script.set("score" + std::to_string(index), score.second);
			}
		}

	private:
		ScriptContext& _script;
		std::vector<std::pair<std::string, int>> _scores;
	};
}

int main(int, char**)
{
	Application application;

	Window window{ application };

	ScriptContext script;

	Tetrium::Game logic;
	script.define("game_pause", [&logic](const ScriptCall&) { logic.pause(); });
	script.define("game_start", [&logic](const ScriptCall& call) { logic.start(call._context.get_int("start_level", 1)); });
	script.define("game_stop", [&logic](const ScriptCall&) { logic.pause(); });
	script.define("game_resume", [&logic](const ScriptCall&) { logic.resume(); });
	script.define("move_down", 1, [&logic](const ScriptCall& call) { logic.set_acceleration(call._args[0]->to_int()); });
	script.define("move_left", 1, [&logic](const ScriptCall& call) { logic.set_left_movement(call._args[0]->to_int()); });
	script.define("move_right", 1, [&logic](const ScriptCall& call) { logic.set_right_movement(call._args[0]->to_int()); });
	script.define("screenshot", [&window](const ScriptCall&) { window.take_screenshot(); });
	script.define("turn_left", [&logic](const ScriptCall&) { logic.turn_left(); });
	script.define("turn_right", [&logic](const ScriptCall&) { logic.turn_right(); });

	ScoreTable score_table{ script };
	script.define("add_score", 2, [&score_table](const ScriptCall& call) { score_table.add_score(call._args[0]->string(), call._args[1]->to_int()); });
	script.define("set_score_count", 1, [&score_table](const ScriptCall& call) { score_table.set_score_count(call._args[0]->to_int()); });

	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_package("tetrium.ypq");
	::make_buttons_texture<16>(storage, "data/textures/buttons.tga");
	::make_cursor_texture<64>(storage, "data/textures/cursor.tga");
	::make_sound(storage, "data/sounds/sound.wav");

	std::optional<AudioManager> audio;
	try
	{
		audio.emplace();
	}
	catch (const InitializationError&)
	{
	}

	ResourceLoader resource_loader{ storage, &window.render_manager(), audio ? &*audio : nullptr };
	Gui gui{ resource_loader, script, "data/gui.ion" };
	gui.on_quit([&window] { window.close(); });

	application.on_update([&script, &gui, &logic](const UpdateEvent& event) {
		if (logic.advance(static_cast<int>(event.milliseconds.count())))
		{
			script.set("score", logic.score());
			script.set("lines", logic.lines());
			script.set("level", logic.level());
			if (logic.has_finished())
				gui.notify("game_over");
		}
	});

	window.on_key_event([&gui](const KeyEvent& event) { gui.process_key_event(event); });
	window.on_render([&gui](RenderPass& pass, const Vector2& cursor) { gui.draw(pass, cursor); });
	window.on_screenshot([](Image&& image) { image.save(Writer{ ::make_screenshot_path() }, ImageFormat::Png); });
	window.on_text_input([&gui](std::string_view text) { gui.process_text_input(text); });

	TetriumGraphics graphics{ window.render_manager() };

	LogoCanvas logo_canvas{ gui };
	gui.bind_canvas("logo", logo_canvas);

	FieldCanvas field_canvas{ logic, graphics };
	gui.bind_canvas("field", field_canvas);

	NextFigureCanvas next_figure_canvas{ logic, graphics };
	gui.bind_canvas("next", next_figure_canvas);

	std::optional<AudioPlayer> audio_player;
	if (audio)
	{
		audio_player.emplace(*audio);
		gui.on_music([&audio_player](const std::shared_ptr<MusicReader>& music) { audio_player->set_music(music); });
	}

	gui.start();
	window.set_title(gui.title());
	if (const auto source = resource_loader.open(gui.icon_path()))
		if (const auto image = Image::load(*source))
			window.set_icon(*image);
	window.show();
	application.run();
}
