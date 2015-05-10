#include "game.h"

#include <yttrium/file.h>
#include <yttrium/gui.h>
#include <yttrium/ion.h>
#include <yttrium/renderer.h>
#include <yttrium/script/context.h>
#include <yttrium/texture.h>

Game::Game()
	: _log_manager("tetrium.log")
	, _allocator("game")
	, _audio(&_allocator)
	, _bindings(&_allocator)
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
		ScriptValue* start_level_value = call.context.find("start_level");
		const auto start_level = start_level_value ? start_level_value->to_int() : 1;
		_game.start(start_level);
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
		if (!_audio.player().is_playing())
			_audio.player().play();
		else
			_audio.player().pause();
	});

	ScriptContext::global().define("pop_scene", 0, 1, [this](const ScriptCall& call)
	{
		const auto scenes_to_pop = !call.args.size() ? 1 : call.args.value(0)->to_int();
		if (scenes_to_pop > 0 && !_window->gui().pop_scenes(scenes_to_pop))
			_window->close();
	});

	ScriptContext::global().define("push_scene", 1, [this](const ScriptCall& call)
	{
		_window->gui().push_scene(call.args.string(0));
	});

	ScriptContext::global().define("set", 2, [this](const ScriptCall& call)
	{
		const ScriptValue* value = call.args.value(0);
		if (value->type() == ScriptValue::Name)
			call.context.set(value->to_string(), call.args.string(1, ScriptArgs::Resolve));
	});

	ScriptContext::global().define("screenshot", [this](const ScriptCall&)
	{
		_window->take_screenshot(String::format(DateTime::now(), "%YY-%MM-%DD_%hh-%mm-%ss.png", &_allocator));
	});

	ScriptContext::global().define("stop_music", [this](const ScriptCall&)
	{
		_audio.player().stop();
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
		if (value->type() == ScriptValue::Name)
			call.context.unset(value->to_string());
	});
}

void Game::run()
{
	Y_LOG("Loading");

	_window = Window::create(*this, &_allocator);
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
	_window->set_size(Size(1024, 768));
	_window->show();

	if (!load_blocks())
		return;

	if (_audio.open())
	{
		IonDocument data(&_allocator);
		if (data.load("data/music.ion"))
		{
			for (const IonValue& value : data.last("music"))
			{
				const IonObject* entry = value.object();
				if (!entry)
					continue;

				StaticString file = entry->last("file").string();
				if (file.is_empty())
					continue;

				AudioPlayer::Settings settings;
				settings.begin = entry->last("begin").string().to_time();
				settings.end = entry->last("end").string().to_time();
				settings.loop = entry->last("loop").string().to_time();
				_audio.player().load(file, settings);
			}
			_audio.player().set_order(AudioPlayer::Random);
			_audio.player().play();
		}
	}

	if (!_window->gui().load("examples/tetrium/gui/gui.ion"))
		return;

	Y_LOG("Starting");

	_window->run();

	Y_LOG("Saving settings");

	File settings_file("tetrium.txt", File::Write | File::Truncate, &_allocator);
	if (settings_file)
	{
		String settings(&_allocator);
		settings << "unbindall\n";
		for (const auto& binding : _bindings.map())
			settings << "bind " << binding.first << " \"" << binding.second.escaped("\\\"", '\\') << "\"\n";
		settings_file.write(settings.text(), settings.size());
	}
}

bool Game::load_blocks()
{
	IonDocument data(&_allocator);
	if (!data.load("examples/tetrium/data/tetrium.ion"))
		return false;

	const IonObject* blocks;
	if (!Ion::read(data, "blocks", blocks))
		return false;

	const StaticString* block_texture_name;
	if (!blocks->last("file", &block_texture_name))
		return false;
	_block_texture = _texture_cache->load_texture_2d(*block_texture_name);
	if (_block_texture)
		_block_texture->set_filter(Texture2D::TrilinearFilter);

	if (!Ion::read(*blocks, "size", _block_size))
		return false;

	const IonNode& block_bases = blocks->last("base");
	if (block_bases.size() != 8)
		return false;

	int index = 0;
	for (const IonValue& value : block_bases)
	{
		if (!Ion::read(value, _block_coords[index++]))
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
	PushTexture push_texture(renderer, _block_texture);
	if (canvas_name == S("field"))
		draw_field(renderer, rect);
	else if (canvas_name == S("next"))
		draw_next_figure(renderer, rect);
}

void Game::on_update(const UpdateEvent& update)
{
	_window->debug_text().clear()
		<< "FPS: " << update.fps << "\n"
		<< "MaxFrameTime: " << update.max_frame_time << "\n"
		<< "Triangles: " << update.triangles << "\n"
		<< "DrawCalls: " << update.draw_calls << "\n"
		<< "TextureChanges: " << update.texture_changes;

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
