#ifndef __TETRIUM_GAME_H
#define __TETRIUM_GAME_H

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>
#include <yttrium/bindings.h>
#include <yttrium/gui/manager.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/renderer/texture_cache.h>
#include <yttrium/terminal.h>
#include <yttrium/time.h>

#include "commands.h"
#include "tetrium.h"

using namespace Yttrium;

class Game
	: public Terminal::Callbacks
	, public Gui::Manager::Callbacks
{
	friend Commands;

public:

	Game(Allocator *allocator);
	~Game();

public:

	bool setup();
	void run();

private:

	bool load();
	void load_music();
	void save_settings();

private: // Terminal::Callbacks.

	void on_cursor_movement(Terminal *terminal, const Dim2 &movement) noexcept override;
	bool on_key_event(Terminal *terminal, Key key, unsigned pressed) noexcept override;

private: // Gui::Manager::Callbacks.

	void on_render_canvas(Renderer *renderer, const StaticString &name, const RectF &rect) noexcept override;

private:

	void draw_field(Renderer *renderer, const RectF &rect);
	void draw_field_blocks(Renderer *renderer, const RectF &rect, const Vector2f &block_size);
	void draw_field_figure(Renderer *renderer, const RectF &rect, const Vector2f &block_size);
	void draw_field_frame(Renderer *renderer, const RectF &rect, const Vector2f &block_size);
	void draw_next_figure(Renderer *renderer, const RectF &rect);
	void set_texture_rectangle(Renderer *renderer, Tetrium::Figure::Type figure_type);

private:

	Allocator       *_allocator;
	ProxyAllocator   _renderer_allocator;
	AudioManager     _audio;
	TerminalPtr      _terminal;
	Renderer         _renderer;
	TextureCachePtr  _texture_cache;
	Gui::ManagerPtr  _gui;
	Bindings         _bindings;

	Texture2DPtr _block_texture;
	float        _block_size;
	Vector2f     _block_coords[8];

	Tetrium::Game _game;
	Timer         _game_timer;

	Commands _commands;
};

#endif // __TETRIUM_GAME_H
