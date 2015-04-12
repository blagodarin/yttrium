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

	_texture_cache = _window->renderer().create_texture_cache();

	_window->set_name("Yttrium RTS example");
	_window->set_size(Size(1024, 768));
	_window->show();

	if (!_window->gui().load("examples/rts/gui.ion"))
		return;

	_window->run();
}

void Game::on_key_event(const KeyEvent& event)
{
	if (!event.pressed)
		return;

	switch (event.key)
	{
	case Key::Escape:
		_window->close();
		break;

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
			90,      // Vertical FOV angle in degrees.
			1, 30)); // Near/far plane distance.

	for (int i = 0; i < 10; ++i)
	{
		const auto z = -4 - 2 * i;
		renderer.draw_cube(Vector4( 2, -2, z), 1);
		renderer.draw_cube(Vector4(-2, -2, z), 1);
		renderer.draw_cube(Vector4( 2,  2, z), 1);
		renderer.draw_cube(Vector4(-2,  2, z), 1);
	}
}

void Game::on_update(const UpdateEvent& update)
{
	_window->debug_text().clear()
		<< "FPS: " << update.fps << "\n"
		<< "MaxFrameTime: " << update.max_frame_time;
}
