#ifndef __EXAMPLES_RTS_GAME_H
#define __EXAMPLES_RTS_GAME_H

#include <yttrium/index_buffer.h>
#include <yttrium/log.h>
#include <yttrium/memory_manager.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/script/manager.h>
#include <yttrium/texture_cache.h>
#include <yttrium/vector.h>
#include <yttrium/vertex_buffer.h>
#include <yttrium/window.h>

#include "model.h"

#include <memory>

using namespace Yttrium;

class Game: public WindowCallbacks
{
public:

	Game();

	void run();

private:

	// WindowCallbacks
	void on_key_event(const KeyEvent& event) override;
	void on_render_canvas(Renderer& renderer, const RectF& rect, const StaticString& canvas_name) override;
	void on_update(const UpdateEvent& update) override;

private:

	MemoryManager _memory_manager;
	LogManager _log_manager;
	ScriptManager _script_manager;
	ProxyAllocator _allocator;

	std::unique_ptr<Window> _window;
	std::unique_ptr<TextureCache> _texture_cache;
	std::unique_ptr<CubeModel> _cube;
	std::unique_ptr<ChessboardModel> _chessboard;

	Vector4 _position = Vector4(-15, 10, 30);
	int _pitch = -15;
	int _yaw = 30;
	int _roll = 0;
};

#endif
