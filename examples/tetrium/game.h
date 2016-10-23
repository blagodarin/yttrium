#ifndef _examples_tetrium_game_h_
#define _examples_tetrium_game_h_

#include <yttrium/audio/manager.h>
#include <yttrium/bindings.h>
#include <yttrium/console.h>
#include <yttrium/gui.h>
#include <yttrium/math/point.h>
#include <yttrium/math/size.h>
#include <yttrium/memory/named_allocator.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/std/map.h>
#include <yttrium/window.h>

#include "blocks.h"
#include "cursor.h"
#include "tetrium.h"

using namespace Yttrium;

class Game : public WindowCallbacks
{
public:
	Game(Storage&);
	~Game();

	void run();

private:
	// WindowCallbacks
	void on_key_event(const KeyEvent&) override;
	void on_render(Renderer&, const PointF& cursor) override;
	void on_screenshot(Image&&) override;
	void on_update(const UpdateEvent&) override;

	void draw_field(Renderer&, const RectF&);
	void draw_field_blocks(Renderer&, const RectF&, const SizeF& block_size);
	void draw_field_figure(Renderer&, const RectF&, const SizeF& block_size);
	void draw_field_frame(Renderer&, const RectF&, const SizeF& block_size);
	void draw_next_figure(Renderer&, const RectF&);
	void set_texture_rect(Renderer&, Tetrium::Figure::Type);

	void update_statistics();

private:
	const Storage& _storage;
	NamedAllocator _allocator{ "game" };
	StdVector<NamedAllocatorInfo> _memory_statistics{ _allocator };
	NamedAllocator _script_allocator{ "script" };
	ScriptContext _script{ &_script_allocator };
	NamedAllocator _audio_allocator{ "audio" };
	AudioManager _audio{ _storage, _audio_allocator };
	NamedAllocator _window_allocator{ "window" };
	Window _window{ "Tetrium", *this, _window_allocator };
	Console _console{ _script, _allocator };
	bool _debug_text_visible = false;
	String _debug_text{ 1024, &_allocator };
	NamedAllocator _resources_allocator{ "resources" };
	ResourceLoader _resource_loader{ _storage, &_window.renderer(), &_audio, _resources_allocator };
	NamedAllocator _gui_allocator{ "gui" };
	Gui _gui{ _resource_loader, _script, _gui_allocator };
	Cursor _cursor{ _window.renderer() };
	Blocks _blocks{ _window.renderer() };
	Bindings _bindings{ _script, _allocator };
	Tetrium::Game _game;
	bool _game_running = false;
	StdMultimap<int, String> _statistics;
};

#endif
