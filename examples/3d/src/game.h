#ifndef _examples_3d_game_h_
#define _examples_3d_game_h_

#include <yttrium/math/euler.h>
#include <yttrium/math/vector.h>
#include <yttrium/memory/named_allocator.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include "model.h"

using namespace Yttrium;

class Game : public WindowCallbacks
{
public:
	void run();

private:
	void on_cursor_movement(const Point& movement) override;
	void on_key_event(const KeyEvent&) override;
	void on_render(Renderer&, const PointF& cursor) override;
	void on_update(const UpdateEvent&) override;

private:
	StdVector<NamedAllocatorInfo> _memory_statistics{ *DefaultAllocator };
	Window _window{ "Yttrium 3D example", *this };
	bool _debug_text_visible = false;
	String _debug_text{ 1024 };
	CubeModel _cube{ _window.renderer() };
	ChessboardModel _chessboard{ _window.renderer() };

	Vector4 _position{ -16, -16, 16 };
	Euler _rotation{ 45, -30, 0 };

	bool _move_forward = false;
	bool _move_backward = false;
	bool _move_left = false;
	bool _move_right = false;
};

#endif
