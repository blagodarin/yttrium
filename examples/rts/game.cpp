#include "game.h"

#include <yttrium/gui.h>
#include <yttrium/matrix.h>
#include <yttrium/renderer.h>
#include <yttrium/time.h>

Game::Game()
	: _log_manager("rts.log")
	, _allocator("game")
{
}

void Game::run()
{
	_window = Window::create(*this, &_allocator);
	if (!_window)
		return;

	_texture_cache = TextureCache::create(_window->renderer());

	_window->set_name("Yttrium RTS example");
	_window->set_size(Size(1024, 768));

	if (!_window->gui().load("examples/rts/gui.ion"))
		return;

	_cube = std::make_unique<CubeModel>(_window->renderer());
	_chessboard = std::make_unique<ChessboardModel>(_window->renderer());

	_window->show();
	_window->run();
}

void Game::on_key_event(const KeyEvent& event)
{
	if (!event.pressed)
		return;

	switch (event.key)
	{
	case Key::Up:
		if (_pitch < 90)
			_pitch += 5;
		break;

	case Key::Down:
		if (_pitch > -90)
			_pitch -= 5;
		break;

	case Key::Left:
		if (_yaw > -180)
			_yaw -= 5;
		else
			_yaw = 175;
		break;

	case Key::Right:
		if (_yaw < 175)
			_yaw += 5;
		else
			_yaw = -180;
		break;

	case Key::Escape:
		_window->close();
		break;

	case Key::A: _position.x -= 1; break;
	case Key::D: _position.x += 1; break;
	case Key::E: _roll += 5; break;
	case Key::F: _position.z -= 1; break;
	case Key::Q: _roll -= 5; break;
	case Key::R: _position.z += 1; break;
	case Key::S: _position.y -= 1; break;
	case Key::W: _position.y += 1; break;

	case Key::Grave:
		_window->set_console_visible(!_window->is_console_visible());
		break;

	case Key::F1:
		_window->set_debug_text_visible(!_window->is_debug_text_visible());
		break;

	case Key::F10: // KDE grabs Key::Print. =(
		_window->take_screenshot(String::format(DateTime::now(), "%YY-%MM-%DD_%hh-%mm-%ss.png", &_allocator));
		break;

	default:
		break;
	}
}

void Game::on_render_canvas(Renderer& renderer, const RectF&, const StaticString& name)
{
	if (name != S("main"))
		return;

	const auto& window_size = renderer.window_size();
	PushProjection projection(renderer,
		Matrix4::perspective(static_cast<float>(window_size.width) / window_size.height,
			90,       // Vertical FOV angle in degrees.
			1, 100)); // Near/far plane distance.

	PushTransformation camera(renderer, Matrix4::camera(_position, _pitch, _yaw, _roll));

	_chessboard->draw(Vector4(0, 0, 0));

	// Center.
	_cube->draw(Vector4(0, 0, 0));

	// X direction -- one cube.
	_cube->draw(Vector4(2, 0, 0));

	// Y direction -- two cubes.
	_cube->draw(Vector4(0, 2, 0));
	_cube->draw(Vector4(0, 4, 0));

	// Z direction -- three cubes.
	_cube->draw(Vector4(0, 0, 2));
	_cube->draw(Vector4(0, 0, 4));
	_cube->draw(Vector4(0, 0, 6));
}

void Game::on_update(const UpdateEvent& update)
{
	_window->debug_text().clear()
		<< "FPS: " << update.fps << "\n"
		<< "MaxFrameTime: " << update.max_frame_time << "\n"
		<< "Triangles: " << update.triangles << "\n"
		<< "DrawCalls: " << update.draw_calls << "\n"
		<< "TextureSwitches: " << update.texture_switches << " (Redundant: " << update.redundant_texture_switches << ")\n"
		<< "X: " << _position.x << ", Y: " << _position.y << ", Z: " << _position.z << "\n"
		<< "Pitch: " << _pitch << ", Yaw: " << _yaw << ", Roll: " << _roll;
}
