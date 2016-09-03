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
#include <yttrium/math/margins.h>
#include <yttrium/memory/temporary_allocator.h>
#include <yttrium/renderer.h>
#include <yttrium/renderer_modifiers.h>
#include <yttrium/script/args.h>
#include <yttrium/script/value.h>
#include <yttrium/texture.h>
#include <yttrium/timer.h>

Game::Game()
	: _allocator("game")
	, _memory_statistics(_allocator)
	, _script_allocator("script")
	, _script(&_script_allocator)
	, _audio_allocator("audio")
	, _window_allocator("window")
	, _bindings(_script, _allocator)
	, _statistics({
			{100000, String("John Placeholder", &_allocator)},
			{50000, String("John Placeholder", &_allocator)},
			{10000, String("John Placeholder", &_allocator)},
			{5000, String("John Placeholder", &_allocator)},
			{1000, String("John Placeholder", &_allocator)},
		}, _allocator)
{
	_script.define("bind", 2, [this](const ScriptCall& call)
	{
		_bindings.bind(call.args.string(0), call.args.string(1));
	});

	_script.define("exit", [this](const ScriptCall&)
	{
		_window->close();
	});

	_script.define("game_pause", [this](const ScriptCall&)
	{
		_game_running = false;
	});

	_script.define("game_start", [this](const ScriptCall& call)
	{
		_game.start(call.context.get_int("start_level", 1));
		_game_running = true;
	});

	_script.define("game_stop", [this](const ScriptCall&)
	{
		_game_running = false;
	});

	_script.define("game_resume", [this](const ScriptCall&)
	{
		_game_running = true;
	});

	_script.define("move_down", [this](const ScriptCall& call)
	{
		if (_game_running)
			_game.set_acceleration(call.function[0] == '+');
	});

	_script.define("move_left", [this](const ScriptCall& call)
	{
		if (_game_running)
			_game.set_left_movement(call.function[0] == '+');
	});

	_script.define("move_right", [this](const ScriptCall& call)
	{
		if (_game_running)
			_game.set_right_movement(call.function[0] == '+');
	});

	_script.define("play_music", [this](const ScriptCall&)
	{
		if (_audio)
		{
			if (!_audio->player().is_playing())
				_audio->player().play();
			else
				_audio->player().pause();
		}
	});

	_script.define("pop_layer", 0, 1, [this](const ScriptCall& call)
	{
		const auto layers_to_pop = call.args.get_int(0, 1);
		if (layers_to_pop > 0 && !_window->gui().pop_layers(layers_to_pop))
			_window->close();
	});

	_script.define("push_layer", 1, [this](const ScriptCall& call)
	{
		_window->gui().push_layer(call.args.string(0));
	});

	_script.define("save_score", [this](const ScriptCall&)
	{
		_statistics.emplace(_game.score(), String("John Placeholder", &_allocator));
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
		TemporaryAllocator<28> allocator(NoAllocator);
		_window->take_screenshot(String(&allocator) << print(DateTime::now(), "%YY-%MM-%DD_%hh-%mm-%ss.png"));
	});

	_script.define("stop_music", [this](const ScriptCall&)
	{
		if (_audio)
			_audio->player().stop();
	});

	_script.define("tgcon", [this](const ScriptCall&)
	{
		_window->set_console_visible(!_window->is_console_visible());
	});

	_script.define("toggle_debug", [this](const ScriptCall&)
	{
		_window->set_debug_text_visible(!_window->is_debug_text_visible());
	});

	_script.define("turn_left", [this](const ScriptCall&)
	{
		if (_game_running)
			_game.turn_left();
	});

	_script.define("turn_right", [this](const ScriptCall&)
	{
		if (_game_running)
			_game.turn_right();
	});

	_script.define("unbindall", [this](const ScriptCall&)
	{
		_bindings.clear();
	});

	_script.define("unset", 1, [this](const ScriptCall& call)
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

	_window = Window::create(_script, *this, _window_allocator);
	if (!_window)
		return;

	_cursor = make_unique<Cursor>(_allocator, _window->renderer());

	ScriptCode::load("tetrium.txt", &_script.allocator()).execute(_script);

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

	if (!load_blocks())
		return;

	_audio = AudioManager::create({}, {}, _audio_allocator);
	if (_audio)
	{
		IonDocument data(_allocator);
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
		}
	}

	if (!_window->gui().load("examples/tetrium/data/gui.ion"))
		return;

	Log() << "Starting";

	if (_audio)
		_audio->player().play();

	_window->show();
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
	IonDocument data(_allocator);
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

	float block_size = 0;
	if (!Ion::get(*blocks, "size", block_size))
		return false;
	_block_size = {block_size, block_size};

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
	if (canvas_name == "field")
		draw_field(renderer, rect);
	else if (canvas_name == "next")
		draw_next_figure(renderer, rect);
}

void Game::on_render_cursor(Renderer&, const PointF& point)
{
	if (!_game_running)
		_cursor->draw(point);
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
		<< "Memory:";
	NamedAllocator::enumerate(_memory_statistics);
	for (const auto& info : _memory_statistics)
	{
		_window->debug_text() << "\n    " << info.name << " = " << info.blocks << "/" << info.allocations;
		if (info.reallocations)
			_window->debug_text() << " (" << info.reallocations << ")";
	}

	if (_game_running)
	{
		_game.advance(update.milliseconds);
		_script.set("score", _game.score());
		_script.set("lines", _game.lines());
		_script.set("level", _game.level());
		if (_game.has_finished())
		{
			_game_running = false;
			_window->gui().push_layer("game_over");
		}
	}
}

void Game::draw_field(Renderer& renderer, const RectF& rect)
{
	const int total_width = 1 + Tetrium::Field::Width + 1;
	const int total_height = 1 + Tetrium::Field::Height + 1;
	const SizeF block_size(rect.width() / total_width, rect.height() / total_height);
	if (_game_running || _game.has_finished())
		draw_field_blocks(renderer, rect, block_size);
	if (_game_running)
		draw_field_figure(renderer, rect, block_size);
	draw_field_frame(renderer, rect, block_size);
}

void Game::draw_field_blocks(Renderer& renderer, const RectF& rect, const SizeF& block_size)
{
	const auto& field = _game.field();
	for (int y = 0; y < Tetrium::Field::Height; ++y)
	{
		for (int x = 0; x < Tetrium::Field::Width; ++x)
		{
			const auto figure_type = field.blocks[y][x];
			if (figure_type != Tetrium::Figure::None)
			{
				set_texture_rect(renderer, figure_type);
				renderer.draw_rect(
					{
						{
							rect.left() + (1 + x) * block_size.width(),
							rect.top() + (Tetrium::Field::Height - y) * block_size.height()
						},
						block_size
					});
			}
		}
	}
}

void Game::draw_field_figure(Renderer& renderer, const RectF& rect, const SizeF& block_size)
{
	static const Vector2 frame_offset(1, Tetrium::Field::Height);

	const Tetrium::Figure& figure = _game.current_figure();
	if (figure.type() == Tetrium::Figure::None)
		return;

	set_texture_rect(renderer, figure.type());
	for (const auto& block : figure.blocks)
	{
		if (block.y < Tetrium::Field::Height * Tetrium::PointsPerRow)
		{
			renderer.draw_rect(
				{
					{
						rect.left() + (frame_offset.x + block.x) * block_size.width(),
						rect.top() + (frame_offset.y - static_cast<float>(block.y) / Tetrium::PointsPerRow) * block_size.height()
					},
					block_size
				});
		}
	}
}

void Game::draw_field_frame(Renderer& renderer, const RectF& rect, const SizeF& block_size)
{
	renderer.set_color({1, 1, 1, 1});
	set_texture_rect(renderer, Tetrium::Figure::None);

	const int total_width = 1 + Tetrium::Field::Width + 1;
	const int total_height = 1 + Tetrium::Field::Height + 1;

	for (int i = 0; i < total_width; ++i)
		renderer.draw_rect({{rect.left() + i * block_size.width(), rect.top()}, block_size});

	for (int i = 1; i < total_height - 1; ++i)
	{
		const auto y = rect.top() + i * block_size.height();
		renderer.draw_rect({{rect.left(), y}, block_size});
		renderer.draw_rect({{rect.left() + (total_width - 1) * block_size.width(), y}, block_size});
	}

	const auto bottom_y = rect.top() + (total_height - 1) * block_size.height();
	for (int i = 0; i < total_width; ++i)
		renderer.draw_rect({{rect.left() + i * block_size.width(), bottom_y}, block_size});
}

void Game::draw_next_figure(Renderer& renderer, const RectF& rect)
{
	if (_game_running || _game.has_finished())
	{
		const Tetrium::Figure& figure = _game.next_figure();
		if (figure.type() != Tetrium::Figure::None)
		{
			set_texture_rect(renderer, figure.type());
			const SizeF block_size(rect.width() / 4, rect.height() / 2);
			for (const auto& block : figure.blocks)
			{
				renderer.draw_rect(
					{
						{
							rect.left() + block.x * block_size.width(),
							rect.top() + (1 - block.y / Tetrium::PointsPerRow) * block_size.height()
						},
						block_size
					});
			}
		}
	}
}

void Game::set_texture_rect(Renderer& renderer, Tetrium::Figure::Type figure_type)
{
	const int figure_index = (figure_type == Tetrium::Figure::None) ? 0 : figure_type + 1;
	renderer.set_texture_rect({_block_coords[figure_index], _block_size}, {});
}

void Game::update_statistics()
{
	int index = 0;
	for (auto i = _statistics.rbegin(); i != _statistics.rend(); ++i)
	{
		++index;
		TemporaryAllocator<32> allocator(NoAllocator);
		_script.set(String(&allocator) << "name" << index, i->second);
		_script.set(String(&allocator) << "score" << index, i->first);
	}
}
