#include "game.h"

#include <yttrium/audio/player.h>
#include <yttrium/date_time.h>
#include <yttrium/file.h>
#include <yttrium/gui.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/utils.h>
#include <yttrium/ion/value.h>
#include <yttrium/log.h>
#include <yttrium/renderer.h>
#include <yttrium/script/context.h>
#include <yttrium/texture.h>
#include <yttrium/timer.h>

Game::Game()
	: _allocator("game")
	, _bindings(&_allocator)
	, _statistics{
		{10000, String("John Placeholder", &_allocator)},
		{5000, String("John Placeholder", &_allocator)},
		{1000, String("John Placeholder", &_allocator)},
		{500, String("John Placeholder", &_allocator)},
		{100, String("John Placeholder", &_allocator)},
	}
{
	ScriptContext::global().define("bind", 2, [this](const ScriptCall& call)
	{
		_bindings.bind(call.args.string(0), call.args.string(1));
	});

	ScriptContext::global().define("exit", [this](const ScriptCall&)
	{
		_window->close();
	});

	ScriptContext::global().define("game_pause", [this](const ScriptCall&)
	{
		_game_running = false;
	});

	ScriptContext::global().define("game_start", [this](const ScriptCall& call)
	{
		_game.start(call.context.get_int("start_level", 1));
		_game_running = true;
	});

	ScriptContext::global().define("game_stop", [this](const ScriptCall&)
	{
		_game_running = false;
	});

	ScriptContext::global().define("game_resume", [this](const ScriptCall&)
	{
		_game_running = true;
	});

	ScriptContext::global().define("move_down", [this](const ScriptCall& call)
	{
		if (_game_running)
			_game.set_acceleration(call.function[0] == '+');
	});

	ScriptContext::global().define("move_left", [this](const ScriptCall& call)
	{
		if (_game_running)
			_game.set_left_movement(call.function[0] == '+');
	});

	ScriptContext::global().define("move_right", [this](const ScriptCall& call)
	{
		if (_game_running)
			_game.set_right_movement(call.function[0] == '+');
	});

	ScriptContext::global().define("play_music", [this](const ScriptCall&)
	{
		if (_audio)
		{
			if (!_audio->player().is_playing())
				_audio->player().play();
			else
				_audio->player().pause();
		}
	});

	ScriptContext::global().define("pop_scene", 0, 1, [this](const ScriptCall& call)
	{
		const auto scenes_to_pop = call.args.get_int(0, 1);
		if (scenes_to_pop > 0 && !_window->gui().pop_scenes(scenes_to_pop))
			_window->close();
	});

	ScriptContext::global().define("push_scene", 1, [this](const ScriptCall& call)
	{
		_window->gui().push_scene(call.args.string(0));
	});

	ScriptContext::global().define("save_score", [this](const ScriptCall&)
	{
		_statistics.emplace(_game.score(), String("John Placeholder", &_allocator));
		_statistics.erase(_statistics.begin());
		update_statistics();
	});

	ScriptContext::global().define("set", 2, [this](const ScriptCall& call)
	{
		const ScriptValue* value = call.args.value(0);
		if (value->type() == ScriptValue::Type::Name)
			call.context.set(value->to_string(), call.args.string(1, ScriptArgs::Resolve));
	});

	ScriptContext::global().define("screenshot", [this](const ScriptCall&)
	{
		_window->take_screenshot(String::format(DateTime::now(), "%YY-%MM-%DD_%hh-%mm-%ss.png", &_allocator));
	});

	ScriptContext::global().define("stop_music", [this](const ScriptCall&)
	{
		if (_audio)
			_audio->player().stop();
	});

	ScriptContext::global().define("tgcon", [this](const ScriptCall&)
	{
		_window->set_console_visible(!_window->is_console_visible());
	});

	ScriptContext::global().define("toggle_debug", [this](const ScriptCall&)
	{
		_window->set_debug_text_visible(!_window->is_debug_text_visible());
	});

	ScriptContext::global().define("turn_left", [this](const ScriptCall&)
	{
		if (_game_running)
			_game.turn_left();
	});

	ScriptContext::global().define("turn_right", [this](const ScriptCall&)
	{
		if (_game_running)
			_game.turn_right();
	});

	ScriptContext::global().define("unbindall", [this](const ScriptCall&)
	{
		_bindings.clear();
	});

	ScriptContext::global().define("unset", 1, [this](const ScriptCall& call)
	{
		const ScriptValue* value = call.args.value(0);
		if (value->type() == ScriptValue::Type::Name)
			call.context.unset(value->to_string());
	});

	update_statistics();
}

void Game::run()
{
	Log() << "Loading";

	_window = Window::create(*this);
	if (!_window)
		return;

	ScriptCode::load("tetrium.txt").execute();

	_bindings.bind_default(Key::_1, "play_music");
	_bindings.bind_default(Key::_2, "stop_music");
	_bindings.bind_default(Key::F1, "toggle_debug");
	_bindings.bind_default(Key::F10, "screenshot"); // KDE grabs Key::Print. =(
	_bindings.bind_default(Key::Grave, "tgcon");

	_bindings.bind_default(Key::A, "+move_left");
	_bindings.bind_default(Key::D, "+move_right");
	_bindings.bind_default(Key::S, "+move_down");
	_bindings.bind_default(Key::Left, "turn_left");
	_bindings.bind_default(Key::Right, "turn_right");

	_texture_cache = TextureCache::create(_window->renderer());

	_game.set_random_seed(Timer::clock());

	_window->set_name("Tetrium");
	_window->set_size({1024, 768});
	_window->show();

	if (!load_blocks())
		return;

	_audio = AudioManager::create();
	if (_audio)
	{
		IonDocument data(&_allocator);
		if (data.load("data/music.ion"))
		{
			for (const IonValue& value : data.root().last("music"))
			{
				const IonObject* entry = value.object();
				if (!entry)
					continue;

				const auto& file_name = Ion::to_string(*entry, "file");
				if (file_name.is_empty())
					continue;

				AudioPlayer::Settings settings;
				settings.begin = Ion::to_string(*entry, "begin").to_time();
				settings.end = Ion::to_string(*entry, "end").to_time();
				settings.loop = Ion::to_string(*entry, "loop").to_time();
				_audio->player().load(file_name, settings);
			}
			_audio->player().set_order(AudioPlayer::Random);
			_audio->player().play();
		}
	}

	if (!_window->gui().load("examples/tetrium/data/gui.ion"))
		return;

	Log() << "Starting";

	_window->run();

	Log() << "Saving settings";

	File settings_file("tetrium.txt", File::Write | File::Truncate, &_allocator);
	if (settings_file)
	{
		String settings(1024, &_allocator);
		settings << "unbindall\n";
		for (const auto& binding : _bindings)
			settings << "bind " << binding.first << " \"" << binding.second.escaped("\\\"", '\\', &_allocator) << "\"\n";
		settings_file.write(settings.text(), settings.size());
	}
}

bool Game::load_blocks()
{
	IonDocument data(&_allocator);
	if (!data.load("examples/tetrium/data/tetrium.ion"))
		return false;

	const IonObject* blocks;
	if (!Ion::get(data.root(), "blocks", blocks))
		return false;

	const StaticString* block_texture_name;
	if (!Ion::get(*blocks, "file", block_texture_name))
		return false;
	_block_texture = _texture_cache->load_texture_2d(*block_texture_name);
	if (_block_texture)
		_block_texture->set_filter(Texture2D::TrilinearFilter);

	if (!Ion::get(*blocks, "size", _block_size))
		return false;

	const IonNode& block_bases = blocks->last("base");
	if (block_bases.size() != 8)
		return false;

	int index = 0;
	for (const IonValue& value : block_bases)
	{
		if (!Ion::get(value, _block_coords[index++]))
			return false;
	}

	return true;
}

void Game::on_key_event(const KeyEvent& event)
{
	if (!event.autorepeat)
		_bindings.call(event.key, event.pressed ? ScriptCode::Do : ScriptCode::Undo);
}

void Game::on_render_canvas(Renderer& renderer, const RectF& rect, const StaticString& canvas_name)
{
	PushTexture push_texture(renderer, _block_texture.get());
	if (canvas_name == S("field"))
		draw_field(renderer, rect);
	else if (canvas_name == S("next"))
		draw_next_figure(renderer, rect);
}

void Game::on_update(const UpdateEvent& update)
{
	_window->debug_text().reserve(1024);
	_window->debug_text().clear()
		<< "FPS: " << update.fps << "\n"
		<< "MaxFrameTime: " << update.max_frame_time << "\n"
		<< "Triangles: " << update.triangles << "\n"
		<< "DrawCalls: " << update.draw_calls << "\n"
		<< "TextureSwitches: " << update.texture_switches << " (Redundant: " << update.redundant_texture_switches << ")\n"
		<< "ShaderSwitches: " << update.shader_switches << " (Redundant: " << update.redundant_shader_switches << ")\n"
		;

	if (_game_running)
	{
		_game.advance(update.milliseconds);
		ScriptContext::global().set("score", _game.score());
		ScriptContext::global().set("lines", _game.lines());
		ScriptContext::global().set("level", _game.level());
		if (_game.has_finished())
		{
			_game_running = false;
			_window->gui().push_scene("game_over");
		}
	}
}

void Game::draw_field(Renderer& renderer, const RectF& rect)
{
	const int total_width = 1 + Tetrium::Field::Width + 1;
	const int total_height = 1 + Tetrium::Field::Height + 1;
	const Vector2 block_size(rect.width() / total_width, rect.height() / total_height);
	draw_field_blocks(renderer, rect, block_size);
	draw_field_figure(renderer, rect, block_size);
	draw_field_frame(renderer, rect, block_size);
}

void Game::draw_field_blocks(Renderer& renderer, const RectF& rect, const Vector2& block_size)
{
	if (_game_running || _game.has_finished())
	{
		const auto& field = _game.field();
		for (int y = 0; y < Tetrium::Field::Height; ++y)
		{
			for (int x = 0; x < Tetrium::Field::Width; ++x)
			{
				const auto figure_type = field.blocks[y][x];
				if (figure_type != Tetrium::Figure::None)
				{
					set_texture_rectangle(renderer, figure_type);
					renderer.draw_rectangle(RectF(
						rect.left() + (1 + x) * block_size.x,
						rect.top() + (Tetrium::Field::Height - y) * block_size.y,
						block_size.x,
						block_size.y));
				}
			}
		}
	}
}

void Game::draw_field_figure(Renderer& renderer, const RectF& rect, const Vector2& block_size)
{
	static const Vector2 frame_offset(1, Tetrium::Field::Height);

	if (!_game_running)
		return;

	const Tetrium::Figure& figure = _game.current_figure();
	if (figure.type() == Tetrium::Figure::None)
		return;

	set_texture_rectangle(renderer, figure.type());
	for (int i = 0; i < Tetrium::Figure::Blocks; ++i)
	{
		if (figure.blocks[i].y < Tetrium::Field::Height * Tetrium::PointsPerRow)
		{
			renderer.draw_rectangle(RectF(
				rect.left() + (frame_offset.x + figure.blocks[i].x) * block_size.x,
				rect.top() + (frame_offset.y - float(figure.blocks[i].y) / Tetrium::PointsPerRow) * block_size.y,
				block_size.x,
				block_size.y));
		}
	}
}

void Game::draw_field_frame(Renderer& renderer, const RectF& rect, const Vector2& block_size)
{
	renderer.set_color(Vector4(1, 1, 1, 1));
	set_texture_rectangle(renderer, Tetrium::Figure::None);

	const int total_width = 1 + Tetrium::Field::Width + 1;
	const int total_height = 1 + Tetrium::Field::Height + 1;

	RectF block(0, 0, block_size.x, block_size.y);

	block.set_top(rect.top());
	for (int i = 0; i < total_width; ++i)
	{
		block.set_left(rect.left() + i * block.width());
		renderer.draw_rectangle(block);
	}

	for (int i = 1; i < total_height - 1; ++i)
	{
		block.set_top(rect.top() + i * block.height());

		block.set_left(rect.left());
		renderer.draw_rectangle(block);

		block.set_left(rect.left() + (total_width - 1) * block.width());
		renderer.draw_rectangle(block);
	}

	block.set_top(rect.top() + (total_height - 1) * block.height());
	for (int i = 0; i < total_width; ++i)
	{
		block.set_left(rect.left() + i * block.width());
		renderer.draw_rectangle(block);
	}
}

void Game::draw_next_figure(Renderer& renderer, const RectF& rect)
{
	if (_game_running || _game.has_finished())
	{
		const Tetrium::Figure& figure = _game.next_figure();

		if (figure.type() != Tetrium::Figure::None)
		{
			set_texture_rectangle(renderer, figure.type());

			float block_width = rect.width() / 4;
			float block_height = rect.height() / 2;

			for (int i = 0; i < Tetrium::Figure::Blocks; ++i)
			{
				renderer.draw_rectangle(RectF(
					rect.left() + figure.blocks[i].x * block_width,
					rect.top() + (1 - figure.blocks[i].y / Tetrium::PointsPerRow) * block_height,
					block_width, block_height));
			}
		}
	}
}

void Game::set_texture_rectangle(Renderer& renderer, Tetrium::Figure::Type figure_type)
{
	const int figure_index = (figure_type == Tetrium::Figure::None) ? 0 : figure_type + 1;
	renderer.set_texture_rectangle(_block_coords[figure_index].x, _block_coords[figure_index].y, _block_size, _block_size);
}

void Game::update_statistics()
{
	int index = 0;
	for (auto i = _statistics.rbegin(); i != _statistics.rend(); ++i)
	{
		++index;

		String name("name", &_allocator);
		name << index;
		ScriptContext::global().set(name, i->second);

		String score("score", &_allocator);
		score << index;
		ScriptContext::global().set(score, i->first);
	}
}