#ifndef __TETRIUM_GAME_H
#define __TETRIUM_GAME_H

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>
#include <yttrium/bindings.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/renderer/texture_cache.h>
#include <yttrium/window.h>
#include <yttrium/time.h>

#include <memory>

#include "commands.h"
#include "tetrium.h"

using namespace Yttrium;

class Game: public Window::Callbacks
{
	friend Commands;

public:

	Game(Allocator* allocator);
	~Game();

public:

	bool setup();
	void run();

private:

	bool load();
	void load_music();
	void save_settings();

	// Window::Callbacks
	void on_cursor_movement(Window& window, const Dim2& movement) override;
	void on_key_event(const KeyEvent& event) override;
	void on_render_canvas(Renderer& renderer, const RectF& rect, const StaticString& canvas_name) override;

	void draw_field(Renderer& renderer, const RectF& rect);
	void draw_field_blocks(Renderer& renderer, const RectF& rect, const Vector2f& block_size);
	void draw_field_figure(Renderer& renderer, const RectF& rect, const Vector2f& block_size);
	void draw_field_frame(Renderer& renderer, const RectF& rect, const Vector2f& block_size);
	void draw_next_figure(Renderer& renderer, const RectF& rect);
	void set_texture_rectangle(Renderer& renderer, Tetrium::Figure::Type figure_type);

private:

	Allocator*       _allocator;
	AudioManager     _audio;
	WindowPtr        _window;
	std::unique_ptr<TextureCache> _texture_cache;
	Bindings         _bindings;

	Texture2DPtr _block_texture;
	float        _block_size;
	Vector2f     _block_coords[8];

	Tetrium::Game _game;
	Timer         _game_timer;

	Commands _commands;
};

#endif // __TETRIUM_GAME_H
