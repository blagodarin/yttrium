#include "game.h"

#include <yttrium/date_time.h>
#include <yttrium/math/matrix.h>
#include <yttrium/renderer.h>
#include <yttrium/renderer_modifiers.h>
#include <yttrium/string_format.h>

#include <cmath>

void Game::run()
{
	_window = Window::create(*this);
	if (!_window)
		return;

	_window->set_name("Yttrium 3D example");

	_cube = make_unique<CubeModel>(*DefaultAllocator, _window->renderer());
	_chessboard = make_unique<ChessboardModel>(*DefaultAllocator, _window->renderer());

	_window->lock_cursor(true);
	_window->show();
	_window->run();
}

void Game::on_cursor_movement(const Point& movement)
{
	_rotation.yaw = wrap(_rotation.yaw - movement.x() / 4.f, -180, 180);
	_rotation.pitch = clamp(_rotation.pitch - movement.y() / 4.f, -90, 90);
}

void Game::on_key_event(const KeyEvent& event)
{
	switch (event.key)
	{
	case Key::Escape:
		if (event.pressed)
			_window->close();
		break;

	case Key::A:
		_move_left = event.pressed;
		break;

	case Key::D:
		_move_right = event.pressed;
		break;

	case Key::S:
		_move_backward = event.pressed;
		break;

	case Key::W:
		_move_forward = event.pressed;
		break;

	case Key::F1:
		if (event.pressed)
			_debug_text_visible = !_debug_text_visible;
		break;

	case Key::F10: // KDE grabs Key::Print. =(
		if (event.pressed)
			_window->take_screenshot(String() << print(DateTime::now(), "%YY-%MM-%DD_%hh-%mm-%ss.png"));
		break;

	default: // To avoid compiler warnings.
		break;
	}
}

void Game::on_render(Renderer& renderer, const PointF&)
{
	{
		Push3D projection(renderer, Matrix4::perspective(renderer.window_size(), 35, .5, 256));

		PushTransformation camera(renderer, Matrix4::camera(_position, _rotation));

		// Center.
		_cube->draw({0, 0, 0});

		// X direction -- one cube.
		_cube->draw({2, 0, 0});

		// Y direction -- two cubes.
		_cube->draw({0, 2, 0});
		_cube->draw({0, 4, 0});

		// Z direction -- three cubes.
		_cube->draw({0, 0, 2});
		_cube->draw({0, 0, 4});
		_cube->draw({0, 0, 6});

		_chessboard->draw({0, 0, 0});
	}
	if (_debug_text_visible)
		renderer.draw_debug_text(_debug_text);
}

void Game::on_update(const UpdateEvent& update)
{
	if (_move_forward != _move_backward || _move_left != _move_right)
	{
		constexpr auto speed = 8.f; // Units per second.
		const auto distance = update.milliseconds * speed / 1000.f;
		const auto offset = (_move_forward || _move_backward) && (_move_left || _move_right) ? distance * M_SQRT1_2 : distance;

		Vector4 movement(0, 0, 0);
		if (_move_forward)
			movement.y += offset;
		else if (_move_backward)
			movement.y -= offset;
		if (_move_left)
			movement.x -= offset;
		else if (_move_right)
			movement.x += offset;

		_position += Matrix4(_rotation) * movement;

		_position.x = clamp(_position.x, -64, 64);
		_position.y = clamp(_position.y, -64, 64);
		_position.z = clamp(_position.z, 1, 64);
	}

	_debug_text.clear()
		<< "FPS: " << update.fps << "\n"
		<< "MaxFrameTime: " << update.max_frame_time << "\n"
		<< "Triangles: " << update.triangles << "\n"
		<< "DrawCalls: " << update.draw_calls << "\n"
		<< "TextureSwitches: " << update.texture_switches << " (Redundant: " << update.redundant_texture_switches << ")\n"
		<< "ShaderSwitches: " << update.shader_switches << " (Redundant: " << update.redundant_shader_switches << ")\n"
		<< "X: " << _position.x << ", Y: " << _position.y << ", Z: " << _position.z << "\n"
		<< "Pitch: " << _rotation.pitch << ", Yaw: " << _rotation.yaw << "\n"
		<< "Memory:";
	NamedAllocator::enumerate(_memory_statistics);
	for (const auto& info : _memory_statistics)
	{
		_debug_text << "\n    " << info.name << " = " << info.blocks << "/" << info.allocations;
		if (info.reallocations)
			_debug_text << " (" << info.reallocations << ")";
		if (info.bytes)
			_debug_text << ", " << info.bytes << " B";
	}
}
