#ifndef __TETRIUM_GAME_H
#define __TETRIUM_GAME_H

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>
#include <yttrium/bindings.h>
#include <yttrium/log.h>
#include <yttrium/memory_manager.h>
#include <yttrium/pointer.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/script/manager.h>
#include <yttrium/texture_cache.h>
#include <yttrium/vector.h>
#include <yttrium/window.h>

#include "tetrium.h"

using namespace Yttrium;

class Game: public WindowCallbacks
{
public:

	Game();

	void run();

private:

	bool load_blocks();

	// WindowCallbacks
	void on_key_event(const KeyEvent& event) override;
	void on_render_canvas(Renderer& renderer, const RectF& rect, const StaticString& canvas_name) override;
	void on_update(const UpdateEvent& update) override;

	void draw_field(Renderer& renderer, const RectF& rect);
	void draw_field_blocks(Renderer& renderer, const RectF& rect, const Vector2& block_size);
	void draw_field_figure(Renderer& renderer, const RectF& rect, const Vector2& block_size);
	void draw_field_frame(Renderer& renderer, const RectF& rect, const Vector2& block_size);
	void draw_next_figure(Renderer& renderer, const RectF& rect);
	void set_texture_rectangle(Renderer& renderer, Tetrium::Figure::Type figure_type);

private:

	MemoryManager _memory_manager;
	LogManager _log_manager;
	ScriptManager _script_manager;
	ProxyAllocator _allocator;

	Pointer<AudioManager> _audio;
	Pointer<Window> _window;
	Pointer<TextureCache> _texture_cache;
	Bindings         _bindings;

	SharedPtr<Texture2D> _block_texture;
	float                _block_size;
	Vector2              _block_coords[8];

	Tetrium::Game _game;
	bool          _game_running = false;
};

#endif // __TETRIUM_GAME_H
