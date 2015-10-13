#include "game.h"

#include <yttrium/date_time.h>
#include <yttrium/gui.h>
#include <yttrium/matrix.h>
#include <yttrium/renderer.h>
#include <yttrium/string.h>
#include <yttrium/string_format.h>

Game::Game(Allocator& allocator)
	: _allocator("game", allocator)
	, _script(&_allocator)
	, _window_allocator("window", allocator)
{
}

void Game::run()
{
	_window = Window::create(_script, *this, _window_allocator);
	if (!_window)
		return;

	_texture_cache = TextureCache::create(_window->renderer());

	_window->set_name("Yttrium RTS example");
	_window->set_size({1024, 768});

	if (!_window->gui().load("examples/rts/data/gui.ion"))
		return;

	_cube = std::make_unique<CubeModel>(_window->renderer());
	_chessboard = std::make_unique<ChessboardModel>(_window->renderer());

	_window->show();
	_window->run();
}

void Game::on_key_event(const KeyEvent& event)
{
	static const int rotation_step = 1;

	if (!event.pressed)
		return;

	switch (event.key)
	{
	case Key::Up:
		if (_pitch < 90)
			_pitch += rotation_step;
		break;

	case Key::Down:
		if (_pitch > -90)
			_pitch -= rotation_step;
		break;

	case Key::Left:
		if (_yaw > -180)
			_yaw -= rotation_step;
		else
			_yaw = 180 - rotation_step;
		break;

	case Key::Right:
		if (_yaw < 180 - rotation_step)
			_yaw += rotation_step;
		else
			_yaw = -180;
		break;

	case Key::Escape:
		_window->close();
		break;

	case Key::A:
		_position.x -= .1;
		break;

	case Key::D:
		_position.x += .1;
		break;

	case Key::E:
		if (_roll < 180 - rotation_step)
			_roll += rotation_step;
		else
			_roll = -180;
		break;

	case Key::F:
		_position.z -= .1;
		break;

	case Key::Q:
		if (_roll > -180)
			_roll -= rotation_step;
		else
			_roll = 180 - rotation_step;
		break;

	case Key::R:
		_position.z += .1;
		break;

	case Key::S:
		_position.y -= .1;
		break;

	case Key::W:
		_position.y += .1;
		break;

	case Key::Grave:
		_window->set_console_visible(!_window->is_console_visible());
		break;

	case Key::F1:
		_window->set_debug_text_visible(!_window->is_debug_text_visible());
		break;

	case Key::F10: // KDE grabs Key::Print. =(
		_window->take_screenshot(String(&_allocator) << print(DateTime::now(), "%YY-%MM-%DD_%hh-%mm-%ss.png"));
		break;

	default:
		break;
	}
}

void Game::on_render_canvas(Renderer& renderer, const RectF&, const StaticString& name)
{
	if (name != "main")
		return;

	const auto& window_size = renderer.window_size();
	Push3D projection(renderer,
		Matrix4::perspective(static_cast<float>(window_size.width) / window_size.height,
			60,       // Vertical FOV angle in degrees.
			1, 100)); // Near/far plane distance.

	PushTransformation camera(renderer, Matrix4::camera(_position, _pitch, _yaw, _roll));

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

void Game::on_update(const UpdateEvent& update)
{
	_window->debug_text().clear()
		<< "FPS: " << update.fps << "\n"
		<< "MaxFrameTime: " << update.max_frame_time << "\n"
		<< "Triangles: " << update.triangles << "\n"
		<< "DrawCalls: " << update.draw_calls << "\n"
		<< "TextureSwitches: " << update.texture_switches << " (Redundant: " << update.redundant_texture_switches << ")\n"
		<< "ShaderSwitches: " << update.shader_switches << " (Redundant: " << update.redundant_shader_switches << ")\n"
		<< "X: " << _position.x << ", Y: " << _position.y << ", Z: " << _position.z << "\n"
		<< "Pitch: " << _pitch << ", Yaw: " << _yaw << ", Roll: " << _roll << "\n"
		;
}
