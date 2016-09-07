#ifndef _examples_3d_game_h_
#define _examples_3d_game_h_

#include <yttrium/gui.h>
#include <yttrium/index_buffer.h>
#include <yttrium/math/euler.h>
#include <yttrium/math/vector.h>
#include <yttrium/memory/named_allocator.h>
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
	void run();

private:
	void on_cursor_movement(const Point& movement) override;
	void on_key_event(const KeyEvent&) override;
	void on_render(Renderer&, const PointF& cursor) override;
	void on_render_canvas(Renderer&, const RectF&, const StaticString& canvas_name) override;
	void on_update(const UpdateEvent&) override;

private:
	StdVector<NamedAllocatorInfo> _memory_statistics { *DefaultAllocator };
	ScriptContext _script;
	UniquePtr<Window> _window;
	UniquePtr<Gui> _gui;
	UniquePtr<CubeModel> _cube;
	UniquePtr<ChessboardModel> _chessboard;

	Vector4 _position = {-16, -16, 16};
	Euler _rotation = {45, -30, 0};

	bool _move_forward = false;
	bool _move_backward = false;
	bool _move_left = false;
	bool _move_right = false;
};

#endif
