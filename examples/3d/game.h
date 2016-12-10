#ifndef _examples_3d_game_h_
#define _examples_3d_game_h_

#include <yttrium/math/euler.h>
#include <yttrium/math/vector.h>
#include <yttrium/memory/named_allocator.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/storage/storage.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include "model.h"

using namespace Yttrium;

class Game : public WindowCallbacks
{
public:
	Game(const Storage& storage) : _storage(storage) {}

	void run();

private:
	void on_cursor_movement(const Point& movement) override;
	void on_key_event(const KeyEvent&) override;
	void on_render(Renderer&, const PointF& cursor) override;
	void on_screenshot(Image&&) override;
	void on_update(const UpdateEvent&) override;

private:
	const Storage& _storage;
	StdVector<NamedAllocatorInfo> _memory_statistics{ *DefaultAllocator };
	Window _window{ "Yttrium 3D example", *this };
	ResourceLoader _resource_loader{ _storage, &_window.renderer() };
	bool _debug_text_visible = false;
	String _debug_text{ 1024 };
	CubeModel _cube{ _window.renderer(), _resource_loader };
	ChessboardModel _chessboard{ _window.renderer(), _resource_loader };

	Vector4 _position{ 16, 16, 16 };
	Euler _rotation{ -135, -30, 0 };

	bool _move_forward = false;
	bool _move_backward = false;
	bool _move_left = false;
	bool _move_right = false;

	unsigned _animation = 0;
};

#endif
