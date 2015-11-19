#ifndef _examples_rts_game_h_
#define _examples_rts_game_h_

#include <yttrium/index_buffer.h>
#include <yttrium/math/vector.h>
#include <yttrium/memory/proxy_allocator.h>
#include <yttrium/script/context.h>
#include <yttrium/texture_cache.h>
#include <yttrium/vertex_buffer.h>
#include <yttrium/window.h>

#include "model.h"

#include <memory>

using namespace Yttrium;

class Game : public WindowCallbacks
{
public:
	Game(Allocator&);

	void run();

private:
	// WindowCallbacks
	void on_key_event(const KeyEvent& event) override;
	void on_render_canvas(Renderer& renderer, const RectF& rect, const StaticString& canvas_name) override;
	void on_update(const UpdateEvent& update) override;

private:
	ProxyAllocator _allocator;
	ScriptContext _script;
	ProxyAllocator _window_allocator;
	Pointer<Window> _window;
	Pointer<TextureCache> _texture_cache;
	std::unique_ptr<CubeModel> _cube;
	std::unique_ptr<ChessboardModel> _chessboard;

	Vector4 _position = {0, -6, 12};
	int _pitch = -60;
	int _yaw = 0;
	int _roll = 0;
};

#endif
