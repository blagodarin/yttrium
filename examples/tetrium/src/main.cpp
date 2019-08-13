//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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
#include <yttrium/audio/utils.h>
#include <yttrium/exceptions.h>
#include <yttrium/gui/gui.h>
#include <yttrium/image/image.h>
#include <yttrium/image/utils.h>
#include <yttrium/logger.h>
#include <yttrium/main.h>
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
#include "graphics.h"

namespace
{
	void make_buttons_texture(Yt::Storage& storage, const std::string& name, size_t button_size)
	{
		constexpr size_t button_styles = 4;
		storage.attach_buffer(name, Yt::make_bgra32_tga(button_size, button_size * button_styles, [button_size](size_t, size_t y) {
			const auto style = y / button_size;
			return Yt::Bgra32{ 0xff, 0x44 * style, 0x44 * style };
		}));
	}

	void make_cursor_texture(Yt::Storage& storage, const std::string& name, size_t size)
	{
		storage.attach_buffer(name, Yt::make_bgra32_tga(size, size, [size](size_t x, size_t y) {
			if (y > 2 * x || 2 * y < x || (y > 2 * (size - x) && x > 2 * (size - y)))
				return Yt::Bgra32{ 0, 0, 0, 0 };
			else
				return Yt::Bgra32{
					y * 0xff / (size - 1),
					x * 0xff / (size - 1),
					(size * size - x * y) * 0xff / (size * size),
				};
		}));
	}

	void make_sound(Yt::Storage& storage, const std::string& name)
	{
		Yt::Buffer buffer;
		{
			constexpr size_t frequency = 44100;
			constexpr size_t duration = frequency / 4; // 0.25 s.

			Yt::Writer writer{ buffer };
			if (Yt::write_wav_header(writer, { Yt::AudioSample::i16, 1, frequency }, duration))
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

	template <typename T, typename... Args>
	auto try_create(Args&&... args) -> decltype(T::create(std::forward<Args>(args)...))
	{
		try
		{
			return T::create(std::forward<Args>(args)...);
		}
		catch (const std::runtime_error& e)
		{
			Yt::Logger::log(e.what());
			return {};
		}
	}

	class LogoCanvas : public Yt::Canvas
	{
	public:
		explicit LogoCanvas(Yt::Gui& gui)
			: _gui{ gui } {}

		void on_draw(Yt::RenderPass& pass, const Yt::RectF& rect, std::chrono::milliseconds screen_time) override
		{
			const auto ms = screen_time.count();
			if (ms >= 4000)
				_gui.notify("finished");
			const auto opacity = std::min(1.f, std::abs(static_cast<float>(ms - 500) / 1500.f - 1.f));
			Yt::PushTexture texture{ pass, nullptr };
			pass.draw_rect(rect, { 0.f, 0.f, 0.f, opacity });
		}

	private:
		Yt::Gui& _gui;
	};

	class FieldCanvas : public Yt::Canvas
	{
	public:
		FieldCanvas(const Tetrium::Game& logic, const TetriumGraphics& graphics)
			: _logic{ logic }, _graphics{ graphics } {}

		void on_draw(Yt::RenderPass& pass, const Yt::RectF& rect, std::chrono::milliseconds) override
		{
			_graphics.draw_field(pass, rect, _logic.field(), _logic.current_figure());
		}

	private:
		const Tetrium::Game& _logic;
		const TetriumGraphics& _graphics;
	};

	class NextFigureCanvas : public Yt::Canvas
	{
	public:
		NextFigureCanvas(const Tetrium::Game& logic, const TetriumGraphics& graphics)
			: _logic{ logic }, _graphics{ graphics } {}

		void on_draw(Yt::RenderPass& pass, const Yt::RectF& rect, std::chrono::milliseconds) override
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
		explicit ScoreTable(Yt::ScriptContext& script)
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
			_scores.resize(static_cast<size_t>(count));
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
		Yt::ScriptContext& _script;
		std::vector<std::pair<std::string, int>> _scores;
	};
}

int ymain(int, char**)
{
	Yt::Logger logger;
	Yt::Application application;

	Yt::Window window{ application };

	Yt::ScriptContext script;

	Tetrium::Game logic;
	script.define("game_pause", [&logic](const Yt::ScriptCall&) { logic.pause(); });
	script.define("game_start", [&logic](const Yt::ScriptCall& call) { logic.start(call._context.get_int("start_level", 1)); });
	script.define("game_stop", [&logic](const Yt::ScriptCall&) { logic.pause(); });
	script.define("game_resume", [&logic](const Yt::ScriptCall&) { logic.resume(); });
	script.define("move_down", 1, [&logic](const Yt::ScriptCall& call) { logic.set_acceleration(call._args[0]->to_int()); });
	script.define("move_left", 1, [&logic](const Yt::ScriptCall& call) { logic.set_left_movement(call._args[0]->to_int()); });
	script.define("move_right", 1, [&logic](const Yt::ScriptCall& call) { logic.set_right_movement(call._args[0]->to_int()); });
	script.define("screenshot", [&window](const Yt::ScriptCall&) { window.take_screenshot(); });
	script.define("turn_left", [&logic](const Yt::ScriptCall&) { logic.turn_left(); });
	script.define("turn_right", [&logic](const Yt::ScriptCall&) { logic.turn_right(); });

	ScoreTable score_table{ script };
	script.define("add_score", 2, [&score_table](const Yt::ScriptCall& call) { score_table.add_score(call._args[0]->string(), call._args[1]->to_int()); });
	script.define("set_score_count", 1, [&score_table](const Yt::ScriptCall& call) { score_table.set_score_count(call._args[0]->to_int()); });

	Yt::Storage storage{ Yt::Storage::UseFileSystem::Never };
	storage.attach_package("tetrium.ypq");
	::make_buttons_texture(storage, "data/textures/buttons.tga", 16);
	::make_cursor_texture(storage, "data/textures/cursor.tga", 64);
	::make_sound(storage, "data/sounds/sound.wav");

	const auto audio = try_create<Yt::AudioManager>();

	Yt::ResourceLoader resource_loader{ storage, &window.render_manager() };
	Yt::Gui gui{ "data/gui.ion", resource_loader, script, audio };
	gui.on_quit([&window] { window.close(); });

	application.on_update([&script, &gui, &logic](std::chrono::milliseconds advance) {
		if (logic.advance(static_cast<int>(advance.count())))
		{
			script.set("score", logic.score());
			script.set("lines", logic.lines());
			script.set("level", logic.level());
			if (logic.has_finished())
				gui.notify("game_over");
		}
	});

	window.on_key_event([&gui](const Yt::KeyEvent& event) { gui.process_key_event(event); });
	window.on_render([&gui](Yt::RenderPass& pass, const Yt::Vector2& cursor, const Yt::RenderReport&) { gui.draw(pass, cursor); });
	window.on_screenshot([](Yt::Image&& image) { image.save_as_screenshot(Yt::ImageFormat::Jpeg); });
	window.on_text_input([&gui](std::string_view text) { gui.process_text_input(text); });

	TetriumGraphics graphics{ window.render_manager() };

	LogoCanvas logo_canvas{ gui };
	gui.bind_canvas("logo", logo_canvas);

	FieldCanvas field_canvas{ logic, graphics };
	gui.bind_canvas("field", field_canvas);

	NextFigureCanvas next_figure_canvas{ logic, graphics };
	gui.bind_canvas("next", next_figure_canvas);

	gui.start();
	window.set_title(gui.title());
	if (const auto source = resource_loader.open(gui.icon_path()))
		if (const auto image = Yt::Image::load(*source))
			window.set_icon(*image);
	window.show();
	application.run();
	return 0;
}
