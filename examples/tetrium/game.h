#ifndef __TETRIUM_GAME_H
#define __TETRIUM_GAME_H

#include <Yttrium/bindings.h>
#include <Yttrium/gui/manager.h>
#include <Yttrium/terminal.h>
#include <Yttrium/time.h>

#include "commands.h"
#include "tetrium.h"

using namespace Yttrium;

class Commands;

class Game
	: public Terminal::Callbacks
	, public Gui::Manager::Callbacks
{
	friend class Commands;

public:

	Game();
	~Game();

public:

	bool setup();
	void run();

private:

	void load_settings();

	void save_settings();

	bool load();

private: // Terminal::Callbacks.

	virtual void on_cursor_movement(Terminal *terminal, const Dim2 &movement) noexcept;
	virtual bool on_key_event(Terminal *terminal, Key key, KeyState state) noexcept;

private: // Gui::Manager::Callbacks.

	virtual void on_render_canvas(Renderer *renderer, const StaticString &name, const RectF &rect) noexcept;

private:

	void draw_field(Renderer *renderer, const RectF &rect);
	void draw_field_blocks(Renderer *renderer, const RectF &rect, const Vector2f &block_size);
	void draw_field_figure(Renderer *renderer, const RectF &rect, const Vector2f &block_size);
	void draw_field_frame(Renderer *renderer, const RectF &rect, const Vector2f &block_size);
	void draw_next_figure(Renderer *renderer, const RectF &rect);
	void set_texture_rectangle(Renderer *renderer, Tetrium::Figure::Type figure_type);

private:

	Terminal         _terminal;
	Renderer         _renderer;
	Gui::ManagerPtr  _gui;
	Bindings         _bindings;

	Texture2D  _block_texture;
	float      _block_size;
	Vector2f   _block_coords[8];

	Tetrium::Game _game;
	Timer         _game_timer;

	Commands _commands;
};

#endif // __TETRIUM_GAME_H
