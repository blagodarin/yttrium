#ifndef _examples_tetrium_game_h_
#define _examples_tetrium_game_h_

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>
#include <yttrium/bindings.h>
#include <yttrium/gui.h>
#include <yttrium/math/point.h>
#include <yttrium/math/size.h>
#include <yttrium/memory/named_allocator.h>
#include <yttrium/script/context.h>
#include <yttrium/std/map.h>
#include <yttrium/texture_cache.h>
#include <yttrium/window.h>

#include "cursor.h"
#include "tetrium.h"

using namespace Yttrium;

class Game : public WindowCallbacks
{
public:
	Game();

	void run();

private:

	bool load_blocks();

	// WindowCallbacks
	void on_key_event(const KeyEvent&) override;
	void on_render(Renderer&, const PointF& cursor) override;
	void on_update(const UpdateEvent&) override;

	void draw_field(Renderer&, const RectF&);
	void draw_field_blocks(Renderer&, const RectF&, const SizeF& block_size);
	void draw_field_figure(Renderer&, const RectF&, const SizeF& block_size);
	void draw_field_frame(Renderer&, const RectF&, const SizeF& block_size);
	void draw_next_figure(Renderer&, const RectF&);
	void set_texture_rect(Renderer&, Tetrium::Figure::Type);

	void update_statistics();

private:
	NamedAllocator _allocator;
	StdVector<NamedAllocatorInfo> _memory_statistics;
	NamedAllocator _script_allocator;
	ScriptContext _script;
	NamedAllocator _audio_allocator;
	UniquePtr<AudioManager> _audio;
	NamedAllocator _window_allocator;
	UniquePtr<Window> _window;
	NamedAllocator _gui_allocator;
	UniquePtr<Gui> _gui;
	UniquePtr<Cursor> _cursor;
	UniquePtr<TextureCache> _texture_cache;
	Bindings _bindings;

	SharedPtr<Texture2D> _block_texture;
	SizeF                _block_size;
	PointF               _block_coords[8];

	Tetrium::Game _game;
	bool          _game_running = false;

	StdMultimap<int, String> _statistics;
};

#endif
