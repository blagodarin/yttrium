#include "game.h"

#include <yttrium/file.h>
#include <yttrium/ion.h>
#include <yttrium/log.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/script/context.h>

#define CHECK(condition) do { if (!(condition)) return false; } while (false)

Game::Game(Allocator *allocator)
	: _allocator(allocator)
	, _renderer_allocator("renderer", allocator)
	, _audio(_allocator)
	, _bindings(allocator)
	, _commands(this)
{
}

Game::~Game()
{
	Y_LOG("Terminating...");
	save_settings();
	_renderer.set_texture(Texture2DPtr()); // Otherwise the renderer won't get deleted.
}

bool Game::setup()
{
	Y_LOG("Loading...");

	_window = Window::open(Dim2(1024, 768), this, _allocator);
	if (_window.is_null())
		return false;

	ScriptContext::global().execute_file("tetrium.txt");

	_bindings.bind_default(Key::_1, "play_music");
	_bindings.bind_default(Key::_2, "stop_music");
	_bindings.bind_default(Key::F10, "snap"); // KDE grabs Key::Print. =(
	_bindings.bind_default(Key::Grave, "tgcon");

	_bindings.bind_default(Key::A, "+move_left");
	_bindings.bind_default(Key::D, "+move_right");
	_bindings.bind_default(Key::S, "+move_down");
	_bindings.bind_default(Key::Left, "turn_left");
	_bindings.bind_default(Key::Right, "turn_right");

	_renderer = _window->create_renderer(&_renderer_allocator);
	_texture_cache = TextureCache::create(_renderer);
	_gui = Gui::create(_renderer, this, _allocator);

	_game.set_random_seed(Timer::clock());

	_window->set_name("Tetrium");
	_window->show();

	load_music();

	load();

	return true;
}

void Game::run()
{
	Y_LOG("Starting...");

	RendererBuiltin renderer_builtin = _renderer.renderer_builtin();

	RateCounter fps;

	fps.start();

	for (; ; )
	{
		// Process window events.

		if (!_window->process_events())
			break;

		// Process game events.

		if (_game_timer.is_started())
		{
			_game.advance(_game_timer.reset());
			ScriptContext::global().set("score", _game.score());
			ScriptContext::global().set("lines", _game.lines());
			ScriptContext::global().set("level", _game.level());
			if (_game.has_finished())
				_gui->push_scene("game_over");
		}

		// Draw frame.

		_renderer.begin_frame();
		_gui->set_cursor(_window->cursor());
		_gui->render();
		_window->draw_console(&renderer_builtin);
		_renderer.end_frame();

		// Update FPS.

		ScriptContext::global().set("fps", fps.rate());
		fps.tick();
	}
}

void Game::save_settings()
{
	Y_LOG("Saving settings...");

	File settings_file("tetrium.txt", File::Write | File::Truncate, _allocator);

	if (settings_file.is_opened())
	{
		String settings("# Generated automatically\n\n", _allocator);

		settings << "unbindall\n";
		const Bindings::Map &bindings = _bindings.map();
		for (Bindings::Map::const_iterator i = bindings.begin(); i != bindings.end(); ++i)
			settings << "bind " << i->first << " \"" << i->second.escaped("\\\"", '\\') << "\"\n";

		const ScriptContext::Archive &archive = ScriptContext::global().archive();
		for (ScriptContext::Archive::const_iterator i = archive.begin(); i != archive.end(); ++i)
			settings << "seta " << i->first << " " << i->second << "\n";

		settings_file.write(settings.text(), settings.size());
	}
}

bool Game::load()
{
	CHECK(_gui->load("examples/tetrium/gui/gui.ion"));

	Ion::Document data(_allocator);

	CHECK(data.load("examples/tetrium/data/tetrium.ion"));

	const Ion::Node &blocks_node = data.last("blocks");
	CHECK(!blocks_node.is_empty() && blocks_node.first()->is_object());
	const Ion::Object *blocks = blocks_node.first()->object();

	const StaticString *block_texture_name;
	CHECK(blocks->last("file", &block_texture_name));
	_block_texture = _texture_cache->load_texture_2d(*block_texture_name);
	if (!_block_texture.is_null())
		_block_texture->set_filter(Texture2D::TrilinearFilter);

	const StaticString *block_size;
	CHECK(blocks->last("size", &block_size));
	CHECK(block_size->to_number(&_block_size));

	const Ion::Node &block_bases = blocks->last("base");
	CHECK(block_bases.size() == 8);
	int index = 0;
	for (const Ion::Value &value: block_bases)
	{
		const Ion::List *entry;

		CHECK(value.get(&entry) && entry->size() == 2);

		float x;
		float y;

		CHECK(entry->first()->string().to_number(&x));
		CHECK(entry->last()->string().to_number(&y));

		_block_coords[index++] = Vector2f(x, y);
	}

	return true;
}

void Game::load_music()
{
	_audio.open(); // NOTE: And what if it fails?

	Ion::Document data(_allocator);

	if (!data.load("data/music.ion"))
		return;

	for (const Ion::Value &value: data.last("music"))
	{
		const Ion::Object *entry = value.object();

		if (!entry)
			continue;

		StaticString file = entry->last("file").string();

		if (!file.is_empty())
		{
			AudioPlayer::Settings settings;

			settings.begin = entry->last("begin").string().to_time();
			settings.end = entry->last("end").string().to_time();
			settings.loop = entry->last("loop").string().to_time();

			_audio.player().load(file, settings);
		}
	}

	_audio.player().set_order(AudioPlayer::Random);
	_audio.player().play();
}

void Game::on_cursor_movement(Window *, const Dim2 &) noexcept
{
}

bool Game::on_key_event(const KeyEvent &event) noexcept
{
	if (_gui->process_key(event))
		return true;

	if (event.pressed <= 1 && _bindings.call(event.key, event.pressed ? ExecutionMode::Do : ExecutionMode::Undo))
		return true;

	return false;
}

void Game::on_render_canvas(Renderer *renderer, const StaticString &name, const RectF &rect) noexcept
{
	renderer->set_texture(_block_texture);

	if (name == S("field"))
	{
		draw_field(renderer, rect);
	}
	else if (name == S("next"))
	{
		draw_next_figure(renderer, rect);
	}
}

void Game::draw_field(Renderer *renderer, const RectF &rect)
{
	int total_width = 1 + Tetrium::Field::Width + 1;
	int total_height = 1 + Tetrium::Field::Height + 1;

	Vector2f block_size(rect.width() / total_width, rect.height() / total_height);

	draw_field_blocks(renderer, rect, block_size);
	draw_field_figure(renderer, rect, block_size);
	draw_field_frame(renderer, rect, block_size);
}

void Game::draw_field_blocks(Renderer *renderer, const RectF &rect, const Vector2f &block_size)
{
	if (_game_timer.is_started() || _game.has_finished())
	{
		const Tetrium::Field &field = _game.field();

		for (int y = 0; y < Tetrium::Field::Height; ++y)
		{
			for (int x = 0; x < Tetrium::Field::Width; ++x)
			{
				Tetrium::Figure::Type figure_type = field.blocks[y][x];
				if (figure_type != Tetrium::Figure::None)
				{
					set_texture_rectangle(renderer, figure_type);
					renderer->draw_rectangle(
						rect.left() + (1 + x) * block_size.x,
						rect.top() + (Tetrium::Field::Height - y) * block_size.y,
						block_size.x,
						block_size.y);
				}
			}
		}
	}
}

void Game::draw_field_figure(Renderer *renderer, const RectF &rect, const Vector2f &block_size)
{
	static const Vector2f frame_offset(1, Tetrium::Field::Height);

	if (_game_timer.is_started())
	{
		const Tetrium::Figure &figure = _game.current_figure();

		if (figure.type() != Tetrium::Figure::None)
		{
			set_texture_rectangle(renderer, figure.type());
			for (int i = 0; i < Tetrium::Figure::Blocks; ++i)
			{
				if (figure.blocks[i].y < Tetrium::Field::Height * Tetrium::PointsPerRow)
				{
					renderer->draw_rectangle(
						rect.left() + (frame_offset.x + figure.blocks[i].x) * block_size.x,
						rect.top() + (frame_offset.y - float(figure.blocks[i].y) / Tetrium::PointsPerRow) * block_size.y,
						block_size.x,
						block_size.y);
				}
			}
		}
	}
}

void Game::draw_field_frame(Renderer *renderer, const RectF &rect, const Vector2f &block_size)
{
	renderer->set_color(1, 1, 1, 1);
	set_texture_rectangle(renderer, Tetrium::Figure::None);

	int total_width = 1 + Tetrium::Field::Width + 1;
	int total_height = 1 + Tetrium::Field::Height + 1;

	RectF block(0, 0, block_size.x, block_size.y);

	block.set_top(rect.top());
	for (int i = 0; i < total_width; ++i)
	{
		block.set_left(rect.left() + i * block.width());
		renderer->draw_rectangle(block);
	}

	for (int i = 1; i < total_height - 1; ++i)
	{
		block.set_top(rect.top() + i * block.height());

		block.set_left(rect.left());
		renderer->draw_rectangle(block);

		block.set_left(rect.left() + (total_width - 1) * block.width());
		renderer->draw_rectangle(block);
	}

	block.set_top(rect.top() + (total_height - 1) * block.height());
	for (int i = 0; i < total_width; ++i)
	{
		block.set_left(rect.left() + i * block.width());
		renderer->draw_rectangle(block);
	}
}

void Game::draw_next_figure(Renderer *renderer, const RectF &rect)
{
	// TODO: Simplify the conditions.

	if (_game_timer.is_started() || _game.has_finished())
	{
		const Tetrium::Figure &figure = _game.next_figure();

		if (figure.type() != Tetrium::Figure::None)
		{
			set_texture_rectangle(renderer, figure.type());

			float block_width = rect.width() / 4;
			float block_height = rect.height() / 2;

			for (int i = 0; i < Tetrium::Figure::Blocks; ++i)
			{
				renderer->draw_rectangle(
					rect.left() + figure.blocks[i].x * block_width,
					rect.top() + (1 - figure.blocks[i].y / Tetrium::PointsPerRow) * block_height,
					block_width, block_height);
			}
		}
	}
}

void Game::set_texture_rectangle(Renderer *renderer, Tetrium::Figure::Type figure_type)
{
	int figure_index = (figure_type == Tetrium::Figure::None) ? 0 : figure_type + 1;
	renderer->set_texture_rectangle(
		_block_coords[figure_index].x, _block_coords[figure_index].y, _block_size, _block_size);
}
