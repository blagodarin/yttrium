#ifndef _examples_3d_game_h_
#define _examples_3d_game_h_

#include <yttrium/math/euler.h>
#include <yttrium/math/vector.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/storage.h>
#include <yttrium/window.h>

#include "model.h"

using namespace Yttrium;

class Game
{
public:
	Game(const Storage&);

	void run();

private:
	void on_key_event(const KeyEvent&);
	void render(Renderer&);
	void update(const UpdateEvent&);

private:
	const Storage& _storage;
	Window _window{ "Yttrium 3D example" };
	ResourceLoader _resource_loader{ _storage, &_window.renderer() };
	bool _debug_text_visible = false;
	std::string _debug_text;
	Model _cube{ _window.renderer(), _resource_loader, "examples/3d/data/cube.obj", "examples/3d/data/cube.material" };
	Model _checkerboard{ _window.renderer(), _resource_loader, "examples/3d/data/checkerboard.obj", "examples/3d/data/checkerboard.material" };

	Vector4 _position{ 9, 9, 9 };
	Euler _rotation{ -135, -27.5, 0 };

	bool _move_forward = false;
	bool _move_backward = false;
	bool _move_left = false;
	bool _move_right = false;

	unsigned _animation = 0;
};

#endif
